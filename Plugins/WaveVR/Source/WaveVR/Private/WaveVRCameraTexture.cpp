// Fill out your copyright notice in the Description page of Project Settings.

#include "WaveVRPrivatePCH.h"
#include "WaveVRCameraTexture.h"
#if WAVEVR_SUPPORTED_PLATFORMS
#include <android/log.h>
#include "wvr_camera.h"
#undef C_TAG
#define C_TAG "WVR_Camera"
#endif

// Sets default values for this component's properties
UWaveVRCameraTexture::UWaveVRCameraTexture()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
    bActive = false;
    timer = 0.F;
    mPixelFormat = EPixelFormat::PF_R8G8B8A8;

	// ...
}


// Called when the game starts
void UWaveVRCameraTexture::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UWaveVRCameraTexture::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UTexture2D*  UWaveVRCameraTexture::getCameraTexture()
{
    if (!bActive)
    {
        mWidth = 640;
        mHeight = 480;

#if WAVEVR_SUPPORTED_PLATFORMS
        WVR_CameraInfo cameraInfo;

        bActive = WVR_StartCamera(&cameraInfo);
        if (bActive == true) {
            if (cameraInfo.imgType == WVR_CameraImageType_SingleEye) {
                mSize = cameraInfo.size;
                mWidth = cameraInfo.width;
                mHeight = cameraInfo.height;
                nativeRawData = (uint8_t *)malloc(mSize);
                mImgType = (EWVR_CameraImageType)WVR_CameraImageType_SingleEye;
                mImgFormat = (EWVR_CameraImageFormat)cameraInfo.imgFormat;
            }
            else if (cameraInfo.imgType == WVR_CameraImageType_DualEye) {
                mSize = cameraInfo.size;
                mWidth = cameraInfo.width;
                mHeight = cameraInfo.height;
                nativeRawData = (uint8_t *)malloc(mSize);
                mImgType = (EWVR_CameraImageType)WVR_CameraImageType_DualEye;
                mImgFormat = (EWVR_CameraImageFormat)cameraInfo.imgFormat;
            }
            else {
                __android_log_print(ANDROID_LOG_ERROR, C_TAG, "No support these image type");
            }
            __android_log_print(ANDROID_LOG_ERROR, C_TAG, "start camera success, getCameraTexture, width = %d, height = %d", mWidth, mHeight);
        }
        else {
            __android_log_print(ANDROID_LOG_ERROR, C_TAG, "start camera fail");
        }
#endif

    //create texture
    CameraTexture = UTexture2D::CreateTransient(mWidth, mHeight, mPixelFormat);
    echoUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, mWidth, mHeight);
    CameraTexture->UpdateResource();
    rawData.Init(FColor(0, 0, 0, 255), mWidth*mHeight);

    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PostPhysics;

    }
    return CameraTexture;
}

void UWaveVRCameraTexture::shutdownCamera()
{
    if (bActive)
    {
        bActive = false;
#if WAVEVR_SUPPORTED_PLATFORMS
        WVR_StopCamera();

        mWidth = 0;
        mHeight = 0;
#endif
    }
}

void UWaveVRCameraTexture::updateCamera(float delta)
{
    timer += delta;
    if (timer >= (1.f / frameRate))
    {
        timer = 0;
        if (CameraTexture && bActive)
        {
#if WAVEVR_SUPPORTED_PLATFORMS
            bool ret = WVR_GetCameraFrameBuffer(nativeRawData, mSize);
            if (nativeRawData != nullptr) {
                // updateTexture(nativeRawData);
                //__android_log_print(ANDROID_LOG_ERROR, "devin", "start SW codec");
                updateTextureG(nativeRawData);
                //__android_log_print(ANDROID_LOG_ERROR, "devin", "end SW codec");
            }
#endif
        }
    }
}

void UWaveVRCameraTexture::updateTexture(uint8_t* data)
{
    if (data == NULL) return;

#if WAVEVR_SUPPORTED_PLATFORMS
    // fill my Texture Region data
    FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;
    RegionData->Texture2DResource = (FTexture2DResource*)CameraTexture->Resource;
    RegionData->MipIndex = 0;
    RegionData->NumRegions = 1;
    RegionData->Regions = echoUpdateTextureRegion;
    RegionData->SrcPitch = (uint32)(4 * mWidth);
    RegionData->SrcBpp = 4;

    uint8_t* yarray = (uint8_t*)data;
    uint8_t* uarray = yarray + (mWidth * mHeight);
    uint8_t* varray = yarray + (mWidth * mHeight) + (mWidth * mHeight) / 4;
    int ptr = 0;
    uint8_t* rgb = new uint8_t[mWidth * mHeight * 4];

    for (int y = 0; y < mHeight; y++) {
        for (int x = 0; x < mWidth; x++) {
            int yy = *(yarray + (y * mWidth) + x);
            int uu = *(uarray + ((y / 2) * (mWidth / 2)) + (x / 2));
            int vv = *(varray + ((y / 2) * (mWidth / 2)) + (x / 2));

            int r = 1.164 * (yy - 16) + 1.596 * (vv - 128);
            int g = 1.164 * (yy - 16) - 0.813 * (vv - 128) - 0.391 * (uu - 128);
            int b = 1.164 * (yy - 16) + 2.018 * (uu - 128);
            rgb[ptr++] = r > 255 ? 255 : r < 0 ? 0 : r;
            rgb[ptr++] = g > 255 ? 255 : g < 0 ? 0 : g;
            rgb[ptr++] = b > 255 ? 255 : b < 0 ? 0 : b;
            rgb[ptr++] = 255;
        }
    }
    RegionData->SrcData = (uint8*)rgb;

    FTexture2DMipMap& Mip = CameraTexture->PlatformData->Mips[0];
    void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
    FMemory::Memcpy(Data, RegionData->SrcData, mWidth * mHeight * 4);
    Mip.BulkData.Unlock();
    CameraTexture->UpdateResource();
    delete[] rgb;
#endif
}

EPixelFormat UWaveVRCameraTexture::getCameraImageFormat() {
    return mPixelFormat;
}

EWVR_CameraImageType UWaveVRCameraTexture::getCameraImageType() {
    EWVR_CameraImageType ret = EWVR_CameraImageType::WVR_CameraImageType_Invalid;
#if WAVEVR_SUPPORTED_PLATFORMS
    if (bActive) {
        ret = mImgType;
    }
#endif
    return ret;
}

int UWaveVRCameraTexture::getCameraImageWidth() {
    int ret = 0;
#if WAVEVR_SUPPORTED_PLATFORMS
    if (bActive) {
        ret = mWidth;
    }
#endif
    return ret;
}

int UWaveVRCameraTexture::getCameraImageHeight() {
    int ret = 0;
#if WAVEVR_SUPPORTED_PLATFORMS
    if (bActive) {
        ret = mHeight;
    }
#endif
    return ret;
}

void UWaveVRCameraTexture::updateTextureG(uint8_t* data)
{
    if (data == NULL) return;

#if WAVEVR_SUPPORTED_PLATFORMS
    // fill my Texture Region data
    FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;
    RegionData->Texture2DResource = (FTexture2DResource*)CameraTexture->Resource;
    RegionData->MipIndex = 0;
    RegionData->NumRegions = 1;
    RegionData->Regions = echoUpdateTextureRegion;
    RegionData->SrcPitch = (uint32)(4 * mWidth);
    RegionData->SrcBpp = 4;

    char* yuv420sp = (char*)data;
    int* rgb = new int[mWidth * mHeight];
    if (!nativeRawData) return;

    int size = mWidth*mHeight;
    int offset = size;

    int u, v, y1, y2, y3, y4;

//    for (int i = 0, k = 0; i < size; i += 2, k += 2) {
    for (int i = 0, k = 0; i < size; i += 2, k += 1) {
        y1 = yuv420sp[i] & 0xff;
        y2 = yuv420sp[i + 1] & 0xff;
        y3 = yuv420sp[mWidth + i] & 0xff;
        y4 = yuv420sp[mWidth + i + 1] & 0xff;

       // u = yuv420sp[offset + k] & 0xff;
       // v = yuv420sp[offset + (mWidth/2 * mHeight/2) + k] & 0xff;
        u = 0;//u - 128;
        v = 0;//v - 128;

        rgb[i] = YUVtoRGB(y1, u, v);
        rgb[i + 1] = YUVtoRGB(y2, u, v);
        rgb[mWidth + i] = YUVtoRGB(y3, u, v);
        rgb[mWidth + i + 1] = YUVtoRGB(y4, u, v);

        if (i != 0 && (i + 2) % mWidth == 0)
            i += mWidth;
    }

    RegionData->SrcData = (uint8*)rgb;

    FTexture2DMipMap& Mip = CameraTexture->PlatformData->Mips[0];
    void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
    FMemory::Memcpy(Data, RegionData->SrcData, mWidth * mHeight * 4);
    Mip.BulkData.Unlock();
    CameraTexture->UpdateResource();

    delete[] rgb;
#endif
}

int UWaveVRCameraTexture::YUVtoRGB(int y, int u, int v)
{
    int r, g, b;
    r = y + (int)1.402f*v;
    g = y - (int)(0.344f*u + 0.714f*v);
    b = y + (int)1.772f*u;
    r = r>255 ? 255 : r<0 ? 0 : r;
    g = g>255 ? 255 : g<0 ? 0 : g;
    b = b>255 ? 255 : b<0 ? 0 : b;
    //return 0x000000ff | (r << 24) | (g << 16) | (b << 8);
    return 0xff000000 | (r << 16) | (g << 8) | b;
}
