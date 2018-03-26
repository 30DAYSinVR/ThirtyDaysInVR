// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "WaveVRPrivatePCH.h"

#include "HeadMountedDisplay.h"
#include "OpenGLDrvPrivate.h"
#include "OpenGLResources.h"
#include "SceneViewExtension.h"
#include "RendererInterface.h"

#include "PoseManagerImp.h"

#include "PostProcess/PostProcessHMD.h"

#include "WaveVRLog.h"

class FWaveVRCustomPresent;
class IWaveVRPlugin;

/**
 * WaveVR Head Mounted Display
 */
class FWaveVRHMD : public IHeadMountedDisplay, public ISceneViewExtension, public TSharedFromThis<FWaveVRHMD, ESPMode::ThreadSafe>
{
private:
	FWaveVRCustomPresent * CustomPresent;

	const bool bUseUnrealDistortion;
	const bool bShouldUseSeparateRenderTarget;

public:
	/** IHeadMountedDisplay interface */
	virtual FName GetDeviceName() const override
	{
		LOG_FUNC();
		static FName DefaultName(TEXT("WaveVR"));
		return DefaultName;
	}

	virtual bool OnStartGameFrame( FWorldContext& WorldContext ) override;
	virtual bool IsHMDConnected() override { LOG_FUNC(); return true;/*return bIsHmdConnected; if in project: LINK */}
	virtual bool IsHMDEnabled() const override;
	virtual void EnableHMD(bool allow = true) override;
	virtual EHMDDeviceType::Type GetHMDDeviceType() const override;
	virtual bool GetHMDMonitorInfo(MonitorInfo&) override;
	virtual void GetFieldOfView(float& OutHFOVInDegrees, float& OutVFOVInDegrees) const override;
	virtual bool DoesSupportPositionalTracking() const override;
	virtual bool HasValidTrackingPosition() override;
	virtual void RebaseObjectOrientationAndPosition(FVector& OutPosition, FQuat& OutOrientation) const override;
	virtual void SetInterpupillaryDistance(float NewInterpupillaryDistance) override;
	virtual float GetInterpupillaryDistance() const override;
	virtual void GetCurrentOrientationAndPosition(FQuat& CurrentOrientation, FVector& CurrentPosition) override;
	virtual class TSharedPtr<ISceneViewExtension, ESPMode::ThreadSafe> GetViewExtension() override;
	virtual void ApplyHmdRotation(APlayerController* PC, FRotator& ViewRotation) override;
	virtual bool UpdatePlayerCamera(FQuat& CurrentOrientation, FVector& CurrentPosition) override;
	virtual bool IsChromaAbCorrectionEnabled() const override;
	virtual bool IsPositionalTrackingEnabled() const override;
	virtual bool IsHeadTrackingAllowed() const override;
	virtual void ResetOrientationAndPosition(float yaw = 0.f) override;
	virtual void OnEndPlay(FWorldContext& InWorldContext) override;
	virtual void DrawDistortionMesh_RenderThread(struct FRenderingCompositePassContext& Context, const FIntPoint& TextureSize) override;
	virtual void SetTrackingOrigin(EHMDTrackingOrigin::Type NewOrigin) override;
	virtual EHMDTrackingOrigin::Type GetTrackingOrigin() override;

	/** IHeadMountedDisplay interface not used*/
	virtual void GetPositionalTrackingCameraProperties(FVector& OutOrigin, FQuat& OutOrientation, float& OutHFOV, float& OutVFOV, float& OutCameraDistance, float& OutNearPlane, float& OutFarPlane) const override { LOG_FUNC(); }
	virtual uint32	GetNumOfTrackingSensors() const override { LOG_FUNC(); return 0; }
	virtual bool GetTrackingSensorProperties(uint8 InSensorIndex, FVector& OutOrigin, FQuat& OutOrientation, float& OutLeftFOV, float& OutRightFOV, float& OutTopFOV, float& OutBottomFOV, float& OutCameraDistance, float& OutNearPlane, float& OutFarPlane) const override { LOG_FUNC(); return false; }
	virtual FVector GetAudioListenerOffset() const override { LOG_FUNC(); return FVector(0.f); }
	virtual void GatherViewExtensions(TArray<TSharedPtr<class ISceneViewExtension, ESPMode::ThreadSafe> >& OutViewExtensions) override { LOG_FUNC(); IHeadMountedDisplay::GatherViewExtensions(OutViewExtensions); }
	virtual class ISpectatorScreenController* GetSpectatorScreenController() override { LOG_FUNC(); return nullptr; }
	virtual class ISpectatorScreenController const* GetSpectatorScreenController() const override { LOG_FUNC(); return nullptr; }
	virtual float GetDistortionScalingFactor() const override { LOG_FUNC(); return 0; }
	virtual float GetLensCenterOffset() const override { LOG_FUNC(); return 0; }
	virtual void GetDistortionWarpValues(FVector4& K) const override { LOG_FUNC(); }
	virtual bool GetChromaAbCorrectionValues(FVector4& K) const override { LOG_FUNC(); return false; }
	virtual bool HasHiddenAreaMesh() const override { LOG_FUNC(); return false; }
	virtual bool HasVisibleAreaMesh() const override { LOG_FUNC(); return false; }
	virtual void DrawHiddenAreaMesh_RenderThread(class FRHICommandList& RHICmdList, EStereoscopicPass StereoPass) const override { LOG_FUNC(); };
	virtual void DrawVisibleAreaMesh_RenderThread(class FRHICommandList& RHICmdList, EStereoscopicPass StereoPass) const override { LOG_FUNC(); };
	virtual void UpdateScreenSettings(const FViewport* InViewport) override { LOG_FUNC(); }
	virtual void UpdatePostProcessSettings(FPostProcessSettings*) override { LOG_FUNC(); }
	virtual void DrawDebug(UCanvas* Canvas) override { LOG_FUNC(); }
	virtual bool HandleInputKey(class UPlayerInput*, const struct FKey& Key, enum EInputEvent EventType, float AmountDepressed, bool bGamepad) override { LOG_FUNC(); return false; }
	virtual bool HandleInputTouch(uint32 Handle, ETouchType::Type Type, const FVector2D& TouchLocation, FDateTime DeviceTimestamp, uint32 TouchpadIndex) override { LOG_FUNC(); return false; }
	virtual void OnBeginPlay(FWorldContext& InWorldContext) override { LOG_FUNC(); }
	virtual bool OnEndGameFrame(FWorldContext& WorldContext) override { LOG_FUNC(); return false; }
	virtual FTexture* GetDistortionTextureLeft() const override { LOG_FUNC(); return NULL; }
	virtual FTexture* GetDistortionTextureRight() const override { LOG_FUNC(); return NULL; }
	virtual FVector2D GetTextureOffsetLeft() const override { LOG_FUNC(); return FVector2D::ZeroVector; }
	virtual FVector2D GetTextureOffsetRight() const override { LOG_FUNC(); return FVector2D::ZeroVector; }
	virtual FVector2D GetTextureScaleLeft() const override { LOG_FUNC(); return FVector2D::ZeroVector; }
	virtual FVector2D GetTextureScaleRight() const override { LOG_FUNC(); return FVector2D::ZeroVector; }
	virtual const float* GetRedDistortionParameters() const override { LOG_FUNC(); return nullptr; }
	virtual const float* GetGreenDistortionParameters() const override { LOG_FUNC(); return nullptr; }
	virtual const float* GetBlueDistortionParameters() const override { LOG_FUNC(); return nullptr; }
	virtual bool NeedsUpscalePostProcessPass() override { LOG_FUNC(); return false; }
	virtual void RecordAnalytics() override { LOG_FUNC(); }
	virtual FString GetVersionString() const override { LOG_FUNC(); return FString(TEXT("GenericHMD")); }
	virtual bool DoesAppUseVRFocus() const override { LOG_FUNC(); return IHeadMountedDisplay::DoesAppUseVRFocus(); }
	virtual bool DoesAppHaveVRFocus() const override { LOG_FUNC(); return IHeadMountedDisplay::DoesAppHaveVRFocus(); }
	virtual void SetupLateUpdate(const FTransform& ParentToWorld, USceneComponent* Component) override { LOG_FUNC(); }
	virtual void ApplyLateUpdate(FSceneInterface* Scene, const FTransform& OldRelativeTransform, const FTransform& NewRelativeTransform) override { LOG_FUNC(); }
	virtual void ResetOrientation(float Yaw = 0.f) override { LOG_FUNC(); }
	virtual void ResetPosition() override { LOG_FUNC(); }
	virtual void SetBaseRotation(const FRotator& BaseRot) override { LOG_FUNC(); }
	virtual FRotator GetBaseRotation() const override { LOG_FUNC(); return FRotator::ZeroRotator; }
	virtual void SetBaseOrientation(const FQuat& BaseOrient) override { LOG_FUNC(); }
	virtual FQuat GetBaseOrientation() const override { LOG_FUNC(); return FQuat::Identity; }
	virtual EHMDWornState::Type GetHMDWornState() override { return EHMDWornState::Unknown; }

	/** IStereoRendering interface */
	virtual bool IsStereoEnabled() const override;
	virtual bool ShouldUseSeparateRenderTarget() const override;
	virtual void UpdateViewport(bool bUseSeparateRenderTarget, const class FViewport& Viewport, class SViewport* = nullptr) override;
	virtual bool AllocateRenderTargetTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, uint32 Flags, uint32 TargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture, FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples = 1) override;
	virtual bool EnableStereo(bool stereo = true) override;
	virtual void AdjustViewRect(EStereoscopicPass StereoPass, int32& X, int32& Y, uint32& SizeX, uint32& SizeY) const override;
	virtual void CalculateStereoViewOffset(const EStereoscopicPass StereoPassType, const FRotator& ViewRotation, const float MetersToWorld, FVector& ViewLocation) override;
	virtual FMatrix GetStereoProjectionMatrix(const EStereoscopicPass StereoPassType, const float FOV) const override;
	virtual void InitCanvasFromView(FSceneView* InView, UCanvas* Canvas) override;
	virtual void GetEyeRenderParams_RenderThread(const struct FRenderingCompositePassContext& Context, FVector2D& EyeToSrcUVScaleValue, FVector2D& EyeToSrcUVOffsetValue) const override;

	/** IStereoRendering interface not used */
	virtual bool IsStereoEnabledOnNextFrame() const override { LOG_FUNC(); return IsStereoEnabled(); }
	virtual FVector2D GetTextSafeRegionBounds() const override { LOG_FUNC(); return FVector2D(0.75f, 0.75f); }
	virtual void CalculateRenderTargetSize(const class FViewport& Viewport, uint32& InOutSizeX, uint32& InOutSizeY) override { LOG_FUNC(); }
	virtual bool NeedReAllocateViewportRenderTarget(const class FViewport& Viewport) override { LOG_FUNC(); return false; }
	virtual bool IsSpectatorScreenActive() const override { LOG_FUNC(); return false; }
	virtual void GetOrthoProjection(int32 RTWidth, int32 RTHeight, float OrthoDistance, FMatrix OrthoProjection[2]) const override { LOG_FUNC(); }
	//virtual void SetScreenPercentage(float InScreenPercentage) override { LOG_FUNC(); }  // Deprecated after 4.15
	//virtual float GetScreenPercentage() const override { LOG_FUNC(); return 0.0f; }  // Deprecated after 4.15
	virtual FRHICustomPresent* GetCustomPresent() override { LOG_FUNC(); return nullptr; }
	virtual uint32 GetNumberOfBufferedFrames() const override { LOG_FUNC(); return 1; }
	virtual IStereoLayers* GetStereoLayers() override { LOG_FUNC(); return nullptr; }
	virtual void UseImplicitHmdPosition(bool bInImplicitHmdPosition) override { LOG_FUNC(); bImplicitHmdPosition = bInImplicitHmdPosition; }
	virtual void SetClippingPlanes(float NCP, float FCP) override { LOG_FUNC(); }
	virtual void RenderTexture_RenderThread(class FRHICommandListImmediate& RHICmdList, class FRHITexture2D* BackBuffer, class FRHITexture2D* SrcTexture) const override { LOG_FUNC(); }

	/** ISceneViewExtension interface */
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override;

	/** ISceneViewExtension interface not used */
	virtual void PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override { LOG_FUNC(); }
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override { LOG_FUNC(); }
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override { LOG_FUNC(); }
	virtual void PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override { LOG_FUNC(); }
	virtual void SetupViewPoint(APlayerController* Player, FMinimalViewInfo& InViewInfo) override { LOG_FUNC(); }
	virtual void SetupViewProjectionMatrix(FSceneViewProjectionData& InOutProjectionData) override { LOG_FUNC(); }
	virtual void PostInitViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override { LOG_FUNC(); }
	virtual void PostInitView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override { LOG_FUNC(); }
	virtual void PostRenderMobileBasePass_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override { LOG_FUNC(); }
	virtual void PostRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override { LOG_FUNC(); }
	virtual void PostRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override { LOG_FUNC(); }
	virtual int32 GetPriority() const override { LOG_FUNC(); return 0; }
	virtual bool UsePostInitView() const override { LOG_FUNC(); return false; }

public:
	bool IsStereoEnabledInternal() const;

	// DistortionCorrection
private:
    /** Helper method to generate index buffer for manual distortion rendering */
    void GenerateDistortionCorrectionIndexBuffer();
    /** Helper method to generate vertex buffer for manual distortion rendering */
    void GenerateDistortionCorrectionVertexBuffer(EStereoscopicPass Eye);
    /** Generates Distortion Correction Points*/
    void SetNumOfDistortionPoints(int32 XPoints, int32 YPoints);

private:
	void ApplicationWillEnterBackgroundDelegate();
	void ApplicationWillDeactivateDelegate();
	void ApplicationHasReactivatedDelegate();
	void ApplicationHasEnteredForegroundDelegate();
	void OnFEngineLoopInitComplete();

	void setDeviceSupportedMaxFPS();
	bool bResumed;
	//bool bNeedReinitEGL;

public:

    FWaveVRHMD(IWaveVRPlugin* WaveVRPlugin);
    virtual ~FWaveVRHMD();

    // distortion mesh
    uint32 DistortionPointsX;
    uint32 DistortionPointsY;
    uint32 NumVerts;
    uint32 NumTris;
    uint32 NumIndices;
    FHMDViewMesh HiddenAreaMeshes[2];
    FHMDViewMesh VisibleAreaMeshes[2];

    uint16* DistortionMeshIndices;
    FDistortionVertex* DistortionMeshVerticesLeftEye;
    FDistortionVertex* DistortionMeshVerticesRightEye;
    //WaveVRDistortion* mWaveVRDistort;
    FSceneViewFamily* mViewFamily;


private:
    //Returns true if initialization successfully, false if not.
    bool Startup();
	void OnPause();
	void OnResume();
    // Shuts down WaveVR
    void Shutdown();

    void pollEvent();
    void processVREvent(WVR_Event_t vrEvent);
    void ResetProjectionMats();

private:
    bool bWVRInitialized; //WVR_Init success.
    bool bIsHmdConnected;
	bool bPlatformSupported;

    bool bHmdEnabled;
    bool bStereoEnabled;

    PoseManagerImp* PoseMngr;

    /** Consider Player's orientation tracking, both HMD's orientation and players controll motion will be accumulated */
    FRotator	DeltaControlRotation;    // same as DeltaControlOrientation but as rotator
    FQuat	DeltaControlOrientation; // same as DeltaControlRotation but as quat

	IWaveVRPlugin* mWaveVRPlugin;

    FMatrix* LeftProjection;
    FMatrix* RightProjection;

protected:
	static FWaveVRHMD * Instance;

public:
	static inline bool IsWVRInitialized() {
		if (Instance != nullptr)
			return Instance->bWVRInitialized;
		else
			return false;
	}

	static inline FWaveVRHMD * GetInstance() {
		if (Instance != nullptr)
			return Instance;
		else
			return nullptr;
	}
};
