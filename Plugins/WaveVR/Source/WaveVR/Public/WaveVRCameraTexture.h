// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WaveVRCameraTexture.generated.h"

UENUM(BlueprintType)
enum class EWVR_CameraImageType : uint8
{
    WVR_CameraImageType_Invalid = 0,
    WVR_CameraImageType_SingleEye = 1,
    WVR_CameraImageType_DualEye = 2,
};

enum class EWVR_CameraImageFormat : uint8
{
    WVR_CameraImageFormat_Invalid = 0,
    WVR_CameraImageFormat_YUV_420 = 1,
    WVR_CameraImageFormat_Grayscale = 2,
};

UCLASS( ClassGroup=(WaveVR), meta=(BlueprintSpawnableComponent) )
class WAVEVR_API UWaveVRCameraTexture : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWaveVRCameraTexture();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void updateTexture(void* data);

    UFUNCTION(BlueprintCallable, Category = "WaveVR|CameraTexture")
        void updateCamera(float delta);

    UFUNCTION(BlueprintCallable, Category = "WaveVR|CameraTexture")
        UTexture2D* getCameraTexture();

    UFUNCTION(BlueprintCallable, Category = "WaveVR|CameraTexture")
        void shutdownCamera();

    UFUNCTION(BlueprintCallable, Category = "WaveVR|CameraTexture")
        EPixelFormat getCameraImageFormat();

    UFUNCTION(BlueprintCallable, Category = "WaveVR|CameraTexture")
        EWVR_CameraImageType getCameraImageType();

    UFUNCTION(BlueprintCallable, Category = "WaveVR|CameraTexture")
        int getCameraImageWidth();

    UFUNCTION(BlueprintCallable, Category = "WaveVR|CameraTexture")
        int getCameraImageHeight();
private:
    bool bActive = false;
    UTexture2D* CameraTexture;
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mSize;
    EWVR_CameraImageType mImgType;
    EWVR_CameraImageFormat mImgFormat;
    TArray<FColor> rawData;
    FUpdateTextureRegion2D *echoUpdateTextureRegion;
    void updateTexture(uint8_t* data);
    void updateTextureG(uint8_t* data);
    int YUVtoRGB(int y, int u, int v);
    const float frameRate = 30.f;
    float timer = 0;
    uint8_t* nativeRawData;
    EPixelFormat mPixelFormat;
};

// Region Data struct
struct FUpdateTextureRegionsData
{
    FTexture2DResource* Texture2DResource;
    int32 MipIndex;
    uint32 NumRegions;
    FUpdateTextureRegion2D* Regions;
    uint32 SrcPitch;
    uint32 SrcBpp;
    uint8* SrcData;
};