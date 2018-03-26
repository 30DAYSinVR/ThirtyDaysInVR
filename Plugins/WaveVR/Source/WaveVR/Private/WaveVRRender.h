#pragma once

#include "UnrealClient.h"

#include "OpenGLDrvPrivate.h"
#include "OpenGLResources.h"

#include "wvr.h"
#include "wvr_device.h"
#include "wvr_events.h"
#include "wvr_render.h"
#include "wvr_projection.h"

class FWVRTextureQueue : public FOpenGLTexture2D
{
public:
	static FWVRTextureQueue* CreateTexture2DSet(
		FOpenGLDynamicRHI* InGLRHI,
		uint32 SizeX,
		uint32 SizeY,
		uint32 InNumSamples,
		EPixelFormat InFormat,
		uint32 InFlags);

	FWVRTextureQueue(
		class FOpenGLDynamicRHI* InGLRHI,
		GLuint InResource,
		GLenum InTarget,
		GLenum InAttachment,
		uint32 InSizeX,
		uint32 InSizeY,
		uint32 InSizeZ,
		uint32 InNumMips,
		uint32 InNumSamples,
		uint32 InNumSamplesTileMem,
		uint32 InArraySize,
		EPixelFormat InFormat,
		bool bInCubemap,
		bool bInAllocatedStorage,
		uint32 InFlags,
		uint8* InTextureRange);

	virtual ~FWVRTextureQueue();

	void SwitchToNextElement();
	void SetIndexAsCurrentRenderTarget();
	GLuint GetRenderTargetResource();
	FTexture2DRHIRef GetRenderTarget();

private:
	FWVRTextureQueue(const FWVRTextureQueue &) = delete;
	FWVRTextureQueue(FWVRTextureQueue &&) = delete;
	FWVRTextureQueue &operator=(const FWVRTextureQueue &) = delete;

	enum { mkRenderTargetTextureNum = 5 };
	FTexture2DRHIRef    mRenderTargetTexture2DRHIRef[mkRenderTargetTextureNum];
	int                 mRenderTargetTexture2DRHIRefIndex;
};

class FWaveVRHMD;
class FWaveVRCustomPresent : public FRHICustomPresent
{
public:
	FWaveVRCustomPresent(FWaveVRHMD* plugin);

public:
	// FRHICustomPresent
	virtual bool Present(int& SyncInterval) override;
	virtual void OnBackBufferResize() override {};

public:
	void DoRenderInitialization();
	bool AllocateRenderTargetTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, uint32 Flags, uint32 TargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture, FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples = 1);
	void UpdateViewport(const FViewport& Viewport, FRHIViewport* InViewportRHI);
	void Reset();
	void Shutdown();

	inline bool IsInitialized() const { return bInitialized && !bNeedReinitRendererAPI; }
	inline TRefCountPtr<FWVRTextureQueue> GetTextureQueue() const { return TextureQueue; }

private:
	void DoRenderInitialization_RenderThread();
	TRefCountPtr<FWVRTextureQueue> TextureQueue;

	void FinishRendering();

protected:
	FWaveVRHMD*	Plugin;
	bool bNeedReinitRendererAPI;
	bool bInitialized;
};
