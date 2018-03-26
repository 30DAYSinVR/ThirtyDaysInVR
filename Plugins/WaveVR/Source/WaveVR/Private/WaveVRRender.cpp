// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
//
#include "WaveVRPrivatePCH.h"
#include "WaveVRHMD.h"
#include "WaveVRRender.h"
#include "WaveVRLog.h"

/* WVR internal API */
#if WAVEVR_SUPPORTED_PLATFORMS
extern "C" bool WVR_EXPORT WVR_IsATWActive();
extern "C" void WVR_EXPORT WVR_SetATWActive(bool isActive, void *anativeWindow = nullptr);
extern "C" void WVR_EXPORT WVR_PauseATW();   // New Api to replace SetATWActive(false)
extern "C" void WVR_EXPORT WVR_ResumeATW();  // New Api to replace SetATWActive(true)
extern "C" void WVR_EXPORT WVR_OnDisable();  // New Api to replace SetATWActive(false) on Unity3D OnDisable
extern "C" void WVR_EXPORT WVR_OnApplicationQuit();  // New Api to handle OnApplicationQuit case
#endif  // WAVEVR_SUPPORTED_PLATFORMS

#if PLATFORM_ANDROID
#include <android_native_app_glue.h>
extern struct android_app* GNativeAndroidApp;
#endif // PLATFORM_ANDROID

void ReportRenderError(WVR_RenderError render_error);

FWVRTextureQueue * FWVRTextureQueue::CreateTexture2DSet(FOpenGLDynamicRHI * InGLRHI, uint32 SizeX, uint32 SizeY, uint32 InNumSamples, EPixelFormat InFormat, uint32 InFlags)
{
	LOG_FUNC();
	const GLenum Target = (InNumSamples > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	return new FWVRTextureQueue(InGLRHI, 0, Target, GL_NONE, SizeX, SizeY, 0, 1, InNumSamples, 1, 1, InFormat, false, false, InFlags, nullptr);
}

FWVRTextureQueue::FWVRTextureQueue(FOpenGLDynamicRHI * InGLRHI, GLuint InResource, GLenum InTarget, GLenum InAttachment,
	uint32 InSizeX, uint32 InSizeY, uint32 InSizeZ, uint32 InNumMips, uint32 InNumSamples,
	uint32 InNumSamplesTileMem, uint32 InArraySize, EPixelFormat InFormat, bool bInCubemap,
	bool bInAllocatedStorage, uint32 InFlags, uint8 * InTextureRange)
	: FOpenGLTexture2D(InGLRHI, InResource, InTarget, InAttachment, 
		InSizeX, InSizeY, InSizeZ, InNumMips, InNumSamples, 
		InNumSamplesTileMem, InArraySize, InFormat, bInCubemap, 
		bInAllocatedStorage, InFlags, InTextureRange, FClearValueBinding::Black)
{
	LOG_FUNC();

	UE_LOG(LogHMD, Log, TEXT("FWVRTextureQueue: InResource %u InTarget %d InAttachment %d InSizeX %u InSizeY %u InSizeZ"
		"InNumMips %u InNumSamples %u, InNumSamplesTileMem %u InArraySize %u InFormat %d bInCubemap %u bInAllocatedStorage %u InFlags %u"),
		InResource, InTarget, InAttachment,
		InSizeX, InSizeY, InSizeZ, InNumMips, InNumSamples,
		InNumSamplesTileMem, InArraySize, (int) InFormat, bInCubemap,
		bInAllocatedStorage, InFlags);

	FRHIResourceCreateInfo CreateInfo;

	for (int i = 0; i < mkRenderTargetTextureNum; ++i)
	{
		mRenderTargetTexture2DRHIRef[i] = InGLRHI->RHICreateTexture2D(InSizeX, InSizeY, InFormat, InNumMips, InNumSamples, InFlags, CreateInfo);

	}

	mRenderTargetTexture2DRHIRefIndex = 0;
	SetIndexAsCurrentRenderTarget();
}

void FWVRTextureQueue::SwitchToNextElement()
{
	LOG_FUNC();
	mRenderTargetTexture2DRHIRefIndex = ((mRenderTargetTexture2DRHIRefIndex + 1) % mkRenderTargetTextureNum);
	SetIndexAsCurrentRenderTarget();
}
void FWVRTextureQueue::SetIndexAsCurrentRenderTarget()
{
	LOG_FUNC();
	Resource = GetRenderTargetResource();
}
GLuint FWVRTextureQueue::GetRenderTargetResource()
{
	LOG_FUNC();
	return *(GLuint*)mRenderTargetTexture2DRHIRef[mRenderTargetTexture2DRHIRefIndex]->GetNativeResource();
}

FTexture2DRHIRef FWVRTextureQueue::GetRenderTarget() {
	LOG_FUNC();
	return mRenderTargetTexture2DRHIRef[mRenderTargetTexture2DRHIRefIndex];
}

FWVRTextureQueue::~FWVRTextureQueue()
{
	LOG_FUNC();
}


FWaveVRCustomPresent::FWaveVRCustomPresent(FWaveVRHMD* plugin) :
	FRHICustomPresent(nullptr),
	Plugin(plugin),
	bNeedReinitRendererAPI(true),
	bInitialized(false)
{
	LOG_FUNC();
}


bool FWaveVRCustomPresent::AllocateRenderTargetTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, uint32 Flags, uint32 TargetableTextureFlags, FTexture2DRHIRef & OutTargetableTexture, FTexture2DRHIRef & OutShaderResourceTexture, uint32 NumSamples)
{
	LOG_FUNC();

	auto OpenGLDynamicRHI = static_cast<FOpenGLDynamicRHI*>(GDynamicRHI);
	check(OpenGLDynamicRHI);

	TextureQueue = FWVRTextureQueue::CreateTexture2DSet(
		OpenGLDynamicRHI,
		SizeX,
		SizeY,
		NumSamples,
		EPixelFormat(Format),
		Flags
	);

	OutShaderResourceTexture = OutTargetableTexture = TextureQueue->GetTexture2D();
	return true;
}

void FWaveVRCustomPresent::FinishRendering()
{
	LOG_FUNC();
	
#if WAVEVR_SUPPORTED_PLATFORMS
	// Submit
	WVR_TextureParams_t param;
	param.id = (WVR_Texture_t)TextureQueue->GetRenderTargetResource();

	WVR_SubmitFrame(WVR_Eye_Left, &param);
	WVR_SubmitFrame(WVR_Eye_Right, &param);
#endif

	TextureQueue->SwitchToNextElement();
}

void FWaveVRCustomPresent::Reset()
{
	LOG_FUNC();
	bInitialized = false;
	bNeedReinitRendererAPI = true;
	// TODO stop ATW in render thread?
}

void FWaveVRCustomPresent::Shutdown()
{
	LOG_FUNC();
	bInitialized = false;
	delete TextureQueue;
	TextureQueue = nullptr;
}

void FWaveVRCustomPresent::UpdateViewport(const FViewport& Viewport, FRHIViewport* InViewportRHI)
{
	LOG_FUNC();
	check(IsInGameThread());
	check(InViewportRHI);

	const FTexture2DRHIRef& RT = Viewport.GetRenderTargetTexture();
	check(IsValidRef(RT));

	InViewportRHI->SetCustomPresent(this);
}

bool FWaveVRCustomPresent::Present(int& SyncInterval)
{
	LOG_FUNC();
	check(IsInRenderingThread());

#if WAVEVR_SUPPORTED_PLATFORMS
	FinishRendering();
#endif

	return false;
}

void FWaveVRCustomPresent::DoRenderInitialization()
{
	LOG_FUNC();

	if (IsInRenderingThread())
	{
		DoRenderInitialization_RenderThread();
	}
	else
	{
		ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(DoRenderInitialization,
			FWaveVRCustomPresent*, pCustomPresent, this,
			{
				pCustomPresent->DoRenderInitialization_RenderThread();
			});
		FlushRenderingCommands();
	}
}

void FWaveVRCustomPresent::DoRenderInitialization_RenderThread()
{
	check(IsInRenderingThread());
	check(!IsInGameThread());

    // call WVR_SetATWActive first and then WVR_RenderInit for our wvr runtime.
	if (bNeedReinitRendererAPI) {
#if WAVEVR_SUPPORTED_PLATFORMS
		WVR_SetATWActive(true, GNativeAndroidApp->window);
#endif
		bNeedReinitRendererAPI = false;
	}

	if (!bInitialized) {
#if WAVEVR_SUPPORTED_PLATFORMS
		WVR_RenderInitParams_t param = { WVR_GraphicsApiType_OpenGL, WVR_RenderConfig_Timewarp_Asynchronous };
		WVR_RenderError render_error = WVR_RenderError_LibNotSupported;
		render_error = WVR_RenderInit(&param);

		if (render_error != WVR_RenderError_None) {
			ReportRenderError(render_error);
			UE_LOG(LogHMD, Log, TEXT("WVR Render Init Failed"));
			return;
		} else {
			UE_LOG(LogHMD, Log, TEXT("WVR Render Init Success"));
		}
#endif
		bInitialized = true;
	}
}
