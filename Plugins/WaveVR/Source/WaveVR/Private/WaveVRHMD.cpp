// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "WaveVRPrivatePCH.h"
#include "RendererPrivate.h"
#include "ScenePrivate.h"
#include "SceneViewport.h"
#include "PostProcess/PostProcessHMD.h"
#include "BatteryStatusEvent.h"
#include "CtrlrSwipeEvent.h"
#include "IpdUpdateEvent.h"
#include "RequestResultObject.h"
#include "PipelineStateCache.h"

#include "WaveVRHMD.h"
#include "WaveVRRender.h"
#include "IWaveVRPlugin.h"

#if PLATFORM_ANDROID
#include "AndroidApplication.h"
#include "AndroidJNI.h"
#include <android/log.h>

#undef LOG_TAG
#define LOG_TAG "WaveHMD"
JavaVM* mJavaVM;
JNIEnv* JavaENV = NULL;
jclass mJavaclazz;
jobject mJavaObj;
#endif

#if WAVEVR_SUPPORTED_PLATFORMS
#include "Android/AndroidEGL.h"
extern "C" void WVR_EXPORT WVR_PauseATW();   // New Api to replace SetATWActive(false)
extern "C" void WVR_EXPORT WVR_ResumeATW();  // New Api to replace SetATWActive(true)
#endif

void ReportError(WVR_InitError error);

TArray<FString> PermissionArr;
TArray<bool> ResultArr;


//---------------------------------------------------
// WaveVR Plugin Implementation
//---------------------------------------------------

class FWaveVRPlugin : public IWaveVRPlugin
{
    /** IHeadMountedDisplayModule implementation */
    virtual TSharedPtr< class IHeadMountedDisplay, ESPMode::ThreadSafe > CreateHeadMountedDisplay() override;

    FString GetModuleKeyName() const {
		LOG_FUNC();
		return FString(TEXT("WaveVR"));
    }

  public:
    FWaveVRPlugin() {
		LOG_FUNC();
#ifdef DEBUG
        UE_LOG(LogHMD, Log, TEXT("FWaveVRPlugin constructor"));
#endif
    }

    //TODO: Test whether it works or not if been removed
    virtual bool IsHMDConnected() override
    {
		LOG_FUNC();
		return true;
    }
};

IMPLEMENT_MODULE( FWaveVRPlugin, WaveVR )

TSharedPtr< class IHeadMountedDisplay, ESPMode::ThreadSafe > FWaveVRPlugin::CreateHeadMountedDisplay()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
    TSharedPtr< FWaveVRHMD, ESPMode::ThreadSafe > WaveVRHMD( new FWaveVRHMD(this) );
    return WaveVRHMD;
}

//---------------------------------------------------
// WaveVR IHeadMountedDisplay Implementation
//---------------------------------------------------

FWaveVRHMD * FWaveVRHMD::Instance = nullptr;

bool FWaveVRHMD::IsHMDEnabled() const
{
	LOG_FUNC();
	// This function will not be used in game, but editor.  If need preview, return true.
	return bHmdEnabled;
}

void FWaveVRHMD::EnableHMD(bool enable)
{
	LOG_FUNC();
	// Only the console command will call this function.  Therefore the bHmdEnabled is initially true.
    bHmdEnabled = enable;

    if (!bHmdEnabled)
    {
        EnableStereo(false);
    }
}

//TODO To have our own device type.  (Need engine modification)
EHMDDeviceType::Type FWaveVRHMD::GetHMDDeviceType() const
{
	LOG_FUNC();
#if !WAVEVR_SUPPORTED_PLATFORMS
	// Use the DT_GoogleVR can elevate the UE's distortion and enable stereo preview
	return EHMDDeviceType::DT_GoogleVR;
#else
	// Elevating GearVR's render flow can prevent EGL problem when suspend and resume.
	// Also do identification in AndroidManifest.UML
	// Not to create window type Surface.
	return EHMDDeviceType::DT_GearVR;
#endif
}

bool FWaveVRHMD::GetHMDMonitorInfo(MonitorInfo& MonitorDesc)
{
	LOG_FUNC();
	if (bWVRInitialized)
    {
        uint32 width = 0, height = 0;
#if WAVEVR_SUPPORTED_PLATFORMS
        WVR_GetRenderTargetSize(&width, &height);
#endif
        MonitorDesc.MonitorName = "";
        MonitorDesc.MonitorId = 0;
        MonitorDesc.DesktopX = 0;
        MonitorDesc.DesktopY = 0;
        MonitorDesc.ResolutionX = width * 2;
        MonitorDesc.ResolutionY = height;
        return true;
    } else {
        MonitorDesc.MonitorName = "";
        MonitorDesc.MonitorId = 0;
        MonitorDesc.DesktopX = 0;
        MonitorDesc.DesktopY = 0;
        MonitorDesc.ResolutionX = 0;
        MonitorDesc.ResolutionY = 0;
    }
    return false;
}

//TODO: re-check
void FWaveVRHMD::GetFieldOfView(float& OutHFOVInDegrees, float& OutVFOVInDegrees) const
{
	LOG_FUNC();
	//Assigned zero means we use projection matrix.
    OutHFOVInDegrees = 0.0f;
    OutVFOVInDegrees = 0.0f;
}

bool FWaveVRHMD::DoesSupportPositionalTracking() const
{
	LOG_FUNC();
    int Dof = PoseMngr->GetNumOfDoF(WVR_DeviceType_HMD);
    switch (Dof) {
        case 6:
            UE_LOG(LogHMD, Log, TEXT("HMD support 6Dof"));
            return true;
        case 3:
            UE_LOG(LogHMD, Log, TEXT("HMD support 3Dof"));
            break;
        case 0:
            UE_LOG(LogHMD, Log, TEXT("HMD DoF abnormal"));
            break;
    }
    return false;
}

bool FWaveVRHMD::HasValidTrackingPosition()
{
	LOG_FUNC();
	return PoseMngr->GetNumOfDoF(WVR_DeviceType_HMD) == 6 && PoseMngr->IsDevicePoseValid(WVR_DeviceType_HMD);
}

void FWaveVRHMD::RebaseObjectOrientationAndPosition(FVector& OutPosition, FQuat& OutOrientation) const
{
	LOG_FUNC();
}

void FWaveVRHMD::SetInterpupillaryDistance(float NewInterpupillaryDistance)
{
	LOG_FUNC();
	UE_LOG(LogHMD, Log, TEXT("NewInterpupillaryDistance %.5f"), NewInterpupillaryDistance);
}

//TODO:
float FWaveVRHMD::GetInterpupillaryDistance() const
{
	LOG_FUNC();
	return 0.064f;
}

void FWaveVRHMD::GetCurrentOrientationAndPosition(FQuat& CurrentOrientation, FVector& CurrentPosition)
{
	LOG_FUNC();
	check(IsInGameThread());
	if (!bWVRInitialized)
		return;

        PoseManagerImp::Device* device = PoseMngr->GetDevice(WVR_DeviceType_HMD);
        CurrentPosition = device->position;
        CurrentOrientation = device->orientation;
}

TSharedPtr<ISceneViewExtension, ESPMode::ThreadSafe> FWaveVRHMD::GetViewExtension()
{
	LOG_FUNC();
	TSharedPtr<FWaveVRHMD, ESPMode::ThreadSafe> ptr(AsShared());
	return StaticCastSharedPtr<ISceneViewExtension>(ptr);
}

void FWaveVRHMD::ApplyHmdRotation(APlayerController* PC, FRotator& ViewRotation)
{
	LOG_FUNC();
	ViewRotation.Normalize();

    const FRotator DeltaRot = ViewRotation - PC->GetControlRotation();
    DeltaControlRotation = (DeltaControlRotation + DeltaRot).GetNormalized();

    // Pitch from other sources is never good, because there is an absolute up and down that must be respected to avoid motion sickness.
    // Same with roll.
    DeltaControlRotation.Pitch = 0;
    DeltaControlRotation.Roll = 0;
    DeltaControlOrientation = DeltaControlRotation.Quaternion();

    PoseManagerImp::Device* device = PoseMngr->GetDevice(WVR_DeviceType_HMD);
    ViewRotation = FRotator(DeltaControlOrientation * device->orientation);
}

bool FWaveVRHMD::UpdatePlayerCamera(FQuat& CurrentOrientation, FVector& CurrentPosition)
{
	LOG_FUNC();
        PoseManagerImp::Device* device = PoseMngr->GetDevice(WVR_DeviceType_HMD);
        CurrentPosition = device->position;
        CurrentOrientation = device->orientation;
	return true;
}

bool FWaveVRHMD::IsChromaAbCorrectionEnabled() const
{
	LOG_FUNC();
	return true;
}

bool FWaveVRHMD::IsPositionalTrackingEnabled() const
{
	LOG_FUNC();
	return PoseMngr->GetNumOfDoF(WVR_DeviceType_HMD) == 6;
}

bool FWaveVRHMD::IsHeadTrackingAllowed() const
{
	LOG_FUNC();
	return bWVRInitialized;
}

void FWaveVRHMD::OnEndPlay(FWorldContext& InWorldContext)
{
	LOG_FUNC();
	EnableStereo(false);
}

bool FWaveVRHMD::OnStartGameFrame(FWorldContext& WorldContext)
{
	LOG_FUNC();
	if (bWVRInitialized == false) return false;

	if (bShouldUseSeparateRenderTarget) {
		if (CustomPresent != nullptr && !CustomPresent->IsInitialized())
			CustomPresent->DoRenderInitialization();
	}

    PoseMngr->UpdatePoses();
    pollEvent();
    return true;
}

void FWaveVRHMD::SetTrackingOrigin(EHMDTrackingOrigin::Type InOrigin)
{
    LOG_FUNC();
    PoseMngr->SetTrackingOriginPoses(InOrigin);
}


EHMDTrackingOrigin::Type FWaveVRHMD::GetTrackingOrigin()
{
    LOG_FUNC();
    return PoseMngr->GetTrackingOriginPoses();
}

void FWaveVRHMD::pollEvent() {
	LOG_FUNC();
	bool ret = false;
#if WAVEVR_SUPPORTED_PLATFORMS
    do
    {
        WVR_Event_t vrevent;
        ret = WVR_PollEventQueue(&vrevent);
        if (ret)
            processVREvent(vrevent);
    } while (ret);
#endif
}

void FWaveVRHMD::processVREvent(WVR_Event_t vrEvent)
{
	LOG_FUNC();
	switch (vrEvent.common.type)
    {
        case WVR_EventType_DeviceConnected:
            UE_LOG(LogHMD, Log, TEXT("WVR_EventType: WVR_EventType Device Connected"));
            bIsHmdConnected = true;
            break;
        case WVR_EventType_DeviceDisconnected:
            UE_LOG(LogHMD, Log, TEXT("WVR_EventType: WVR_EventType Device Disconnected"));
            bIsHmdConnected = false;
            break;
        case WVR_EventType_BatteryStatus_Update:
            UE_LOG(LogHMD, Log, TEXT("WVR_EventType: WVR_EventType battery status updated"));
            UBatteryStatusEvent::onBatteryStatusUpdateNative.Broadcast();
            break;
        case WVR_EventType_IpdChanged:
            UE_LOG(LogHMD, Log, TEXT("WVR_EventType: WVR_EventType IPD updated"));
            UIpdUpdateEvent::onIpdUpdateNative.Broadcast();
            break;
        case WVR_EventType_TouchpadSwipe_LeftToRight:
            UE_LOG(LogHMD, Log, TEXT("WVR_EventType: touch pad swipe (left to right)"));
            UCtrlrSwipeEvent::onCtrlrSwipeLtoRUpdateNative.Broadcast();
            break;
        case WVR_EventType_TouchpadSwipe_RightToLeft:
            UE_LOG(LogHMD, Log, TEXT("WVR_EventType: touch pad swipe (right to left)"));
            UCtrlrSwipeEvent::onCtrlrSwipeRtoLUpdateNative.Broadcast();
            break;
        case WVR_EventType_TouchpadSwipe_UpToDown:
            UE_LOG(LogHMD, Log, TEXT("WVR_EventType: touch pad swipe (up to down)"));
            UCtrlrSwipeEvent::onCtrlrSwipeUtoDUpdateNative.Broadcast();
            break;
        case WVR_EventType_TouchpadSwipe_DownToUp:
            UE_LOG(LogHMD, Log, TEXT("WVR_EventType: touch pad swipe (down to up)"));
            UCtrlrSwipeEvent::onCtrlrSwipeDtoUUpdateNative.Broadcast();
            break;
    }
}

void FWaveVRHMD::ResetOrientationAndPosition(float yaw)
{
	LOG_FUNC();
#if WAVEVR_SUPPORTED_PLATFORMS
    WVR_TriggerRecenter();
#endif
}

bool FWaveVRHMD::IsStereoEnabledInternal() const
{
	// Internal function will not use the LOG_FUNC()
	return bStereoEnabled && bHmdEnabled;
}

bool FWaveVRHMD::IsStereoEnabled() const
{
	LOG_FUNC();
    return bStereoEnabled && bHmdEnabled;
}


bool FWaveVRHMD::ShouldUseSeparateRenderTarget() const
{
	LOG_FUNC();
#if !WAVEVR_SUPPORTED_PLATFORMS
	if (!WAVEVR_SUPPORTED_PLATFORMS)
		return false;
#endif
	return bShouldUseSeparateRenderTarget;
}

void FWaveVRHMD::UpdateViewport(bool bUseSeparateRenderTarget, const class FViewport& Viewport, class SViewport*)
{
	LOG_FUNC();
	//UE_LOG(LogHMD, Log, TEXT("UpdateViewport bUseSeparateRenderTarget = %d"), bUseSeparateRenderTarget);
    FRHIViewport* const ViewportRHI = Viewport.GetViewportRHI().GetReference();
    if (!IsStereoEnabledInternal()) {
        if (!bUseSeparateRenderTarget)
        {
            ViewportRHI->SetCustomPresent(nullptr);
			return;
        }
    }
	if (bUseSeparateRenderTarget)
		CustomPresent->UpdateViewport(Viewport, ViewportRHI);
}

bool FWaveVRHMD::AllocateRenderTargetTexture(
	uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, uint32 Flags,
	uint32 TargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture,
	FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples)
{
	LOG_FUNC();
	return CustomPresent->AllocateRenderTargetTexture(
		Index,
		SizeX,
		SizeY,
		Format,
		NumMips,
		Flags,
		TargetableTextureFlags,
		OutTargetableTexture,
		OutShaderResourceTexture,
		NumSamples);
}

bool FWaveVRHMD::EnableStereo(bool bStereo)
{
	LOG_FUNC();
	// Only Enabled if the develop check the 'Start in VR' from project settings.  Therefore the bStereoEnabled could be initiallized true.
	UE_LOG(LogHMD, Log, TEXT("EnableStereo = %d"), bStereo);

	bStereoEnabled = bStereo;
	return bStereoEnabled;
}

void FWaveVRHMD::AdjustViewRect(EStereoscopicPass StereoPass, int32& X, int32& Y, uint32& SizeX, uint32& SizeY) const
{
	LOG_FUNC();
	SizeX = SizeX / 2;
	if (StereoPass == eSSP_RIGHT_EYE)
	{
		X += SizeX;
	}
	//UE_LOG(LogHMD, Log, TEXT("AdjustViewRect eye %d (%d, %d, %d, %d)"), (int)StereoPass, X, Y, SizeX, SizeY);
}

void FWaveVRHMD::CalculateStereoViewOffset(const enum EStereoscopicPass StereoPassType, const FRotator& ViewRotation, const float WorldToMeters, FVector& ViewLocation)
{
	LOG_FUNC();
#if WAVEVR_SUPPORTED_PLATFORMS
	if (StereoPassType != eSSP_FULL)
    {
        WVR_Eye hmdEye = (StereoPassType == eSSP_LEFT_EYE) ? WVR_Eye_Left : WVR_Eye_Right;
        WVR_Matrix4f_t HEADFormEYE = WVR_GetTransformFromEyeToHead(hmdEye);

        // grab the eye position, currently ignoring the rotation supplied by GetHeadFromEyePose()
        FVector TotalOffset = FVector(-HEADFormEYE.m[2][3], HEADFormEYE.m[0][3], HEADFormEYE.m[1][3]) * WorldToMeters;

        ViewLocation += ViewRotation.Quaternion().RotateVector(TotalOffset);
    }
#else
	if (StereoPassType != eSSP_FULL)
	{
		float EyeOffset = 3.20000005f;
		const float PassOffset = (StereoPassType == eSSP_LEFT_EYE) ? EyeOffset : -EyeOffset;
		ViewLocation += ViewRotation.Quaternion().RotateVector(FVector(0, PassOffset, 0));
	}
#endif
}

FMatrix FWaveVRHMD::GetStereoProjectionMatrix(const enum EStereoscopicPass StereoPassType, const float FOV) const
{
	LOG_FUNC();
	check(IsStereoEnabledInternal());

    FMatrix* Mat = nullptr;
    if (StereoPassType == eSSP_LEFT_EYE) {
        Mat = LeftProjection;
    } else if (StereoPassType == eSSP_RIGHT_EYE){
        Mat = RightProjection;
    }
    return *Mat;
}

void FWaveVRHMD::InitCanvasFromView(FSceneView* InView, UCanvas* Canvas)
{
	LOG_FUNC();
}

FWaveVRHMD::FWaveVRHMD(IWaveVRPlugin* WaveVRPlugin) :
#if WITH_EDITOR
	bUseUnrealDistortion(true),
#else
	bUseUnrealDistortion(false),
#endif
	// If use Unreal Distortion should not use separate render target
	bShouldUseSeparateRenderTarget(!bUseUnrealDistortion),
	bWVRInitialized(false),
	bIsHmdConnected(false),
	bPlatformSupported(WAVEVR_SUPPORTED_PLATFORMS),
	bHmdEnabled(true),
	bStereoEnabled(false),
	PoseMngr(NULL),
	DeltaControlRotation(FRotator::ZeroRotator),
	DeltaControlOrientation(FQuat::Identity),
	DistortionPointsX(40),
	DistortionPointsY(40),
	NumVerts(0),
	NumTris(0),
	NumIndices(0),
	DistortionMeshIndices(nullptr),
	DistortionMeshVerticesLeftEye(nullptr),
	DistortionMeshVerticesRightEye(nullptr),
	mWaveVRPlugin(WaveVRPlugin),
	bResumed(false),
	//bNeedReinitEGL(false),
	LeftProjection(nullptr),
	RightProjection(nullptr)
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	Startup();

#define LOCAL_EOL ", "  // " \n"
	UE_LOG(LogHMD, Log, TEXT("Initialized variable:" LOCAL_EOL
		"bUseUnrealDistortion %d" LOCAL_EOL
		"bShouldUseSeparateRenderTarget %d" LOCAL_EOL
		"bWVRInitialized %d" LOCAL_EOL
		"bIsHmdConnected %d" LOCAL_EOL
		"bPlatformSupported %d" LOCAL_EOL
		"bHmdEnabled %d" LOCAL_EOL
		"bStereoEnabled %d" LOCAL_EOL
	), bUseUnrealDistortion, bShouldUseSeparateRenderTarget, 
		bWVRInitialized, bIsHmdConnected, bPlatformSupported,
		bHmdEnabled, bStereoEnabled);
#undef LOCAL_EOL
}

FWaveVRHMD::~FWaveVRHMD()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	Shutdown();
}

void ReportError(WVR_InitError error)
{
    switch (error)
    {
        case WVR_InitError_None:
            break;
        case WVR_InitError_NotInitialized:
            UE_LOG(LogHMD, Log, TEXT("WaveVR: Not initialized"));
            break;
        case WVR_InitError_Unknown:
            UE_LOG(LogHMD, Log, TEXT("WaveVR: Unknown error during initializing"));
            break;
        default:
            //UE_LOG(LogHMD, Log, TEXT("WaveVR: InitError default case"));
            break;
    }
}

void ReportRenderError(WVR_RenderError render_error)
{
    switch (render_error)
    {
        case WVR_RenderError_None:
            UE_LOG(LogHMD, Log, TEXT("WaveVR: RenderError: None"));
            break;
        case WVR_RenderError_RuntimeInitFailed:
            UE_LOG(LogHMD, Log, TEXT("WaveVR: RenderError: RuntimeInitFailed"));
            break;
        case WVR_RenderError_ContextSetupFailed:
            UE_LOG(LogHMD, Log, TEXT("WaveVR: RenderError: ContextSetupFailed"));
            break;
        case WVR_RenderError_DisplaySetupFailed:
            UE_LOG(LogHMD, Log, TEXT("WaveVR: RenderError: DisplaySetupFailed"));
            break;
        case WVR_RenderError_LibNotSupported:
            UE_LOG(LogHMD, Log, TEXT("WaveVR: RenderError: LibNotSupported"));
            break;
        case WVR_RenderError_NullPtr:
            UE_LOG(LogHMD, Log, TEXT("WaveVR: RenderError: NullPtr"));
            break;
        default:
            //UE_LOG(LogHMD, Log, TEXT("WaveVR: RenderError default case"));
            break;
    }
}
void FWaveVRHMD::setDeviceSupportedMaxFPS()
{
#if WAVEVR_SUPPORTED_PLATFORMS
    WVR_RenderProps props;
    if (WVR_GetRenderProps(&props)) {
        float fps = props.refreshRate;
        UE_LOG(LogHMD, Log, TEXT("Set FreshRate as %f"), fps);
        GEngine->SetMaxFPS(fps);
        //IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("t.MaxFPS"));
        //CVar->Set(fps);
    }
    else {
            UE_LOG(LogHMD, Log, TEXT("Get device FreshRate error! Not success!"));
    }
#endif
}

bool FWaveVRHMD::Startup()
{
	LOG_FUNC();
#if WAVEVR_SUPPORTED_PLATFORMS
	WVR_InitError error =
		WVR_Init(WVR_AppType_VRContent);
    if (error != WVR_InitError_None)
    {
        ReportError(error);
        WVR_Quit();
        UE_LOG(LogHMD, Log, TEXT("WVR_Quit"));
        return false;
    } 


	//UGameUserSettings* Settings = GetGameUserSettings();
	//if (Settings != nullptr)
	//{
	//	LOGD(LogHMD, "RequestResolutionChange")
	//	{
	//		uint32 width = 0, height = 0;
	//		WVR_GetRenderTargetSize(&width, &height);
	//		Settings->RequestResolutionChange(width * 2, height, EWindowMode::Type::Windowed, false);
	//	}
	//}

    pollEvent(); //get device connect info.
#endif
    PoseMngr = PoseManagerImp::GetInstance();
    ResetProjectionMats();

	FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddRaw(this, &FWaveVRHMD::ApplicationWillEnterBackgroundDelegate);
	//calls to this delegate are often (always?) paired with a call to ApplicationWillEnterBackgroundDelegate(), but cover the possibility that only this delegate is called
	FCoreDelegates::ApplicationWillDeactivateDelegate.AddRaw(this, &FWaveVRHMD::ApplicationWillDeactivateDelegate);
	FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddRaw(this, &FWaveVRHMD::ApplicationHasEnteredForegroundDelegate);
	//calls to this delegate are often (always?) paired with a call to ApplicationHasEnteredForegroundDelegate(), but cover the possibility that only this delegate is called
	FCoreDelegates::ApplicationHasReactivatedDelegate.AddRaw(this, &FWaveVRHMD::ApplicationHasReactivatedDelegate);
	FCoreDelegates::OnFEngineLoopInitComplete.AddRaw(this, &FWaveVRHMD::OnFEngineLoopInitComplete);

	if (bUseUnrealDistortion)
		SetNumOfDistortionPoints(40, 40);
	else
		CustomPresent = new FWaveVRCustomPresent(this);

	if (bPlatformSupported)
		bWVRInitialized = true;

	Instance = this;
	return true;
}

void FWaveVRHMD::OnPause()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	if (!bResumed)
		return;
	bResumed = false;

	if (bWVRInitialized)
	{
#if WAVEVR_SUPPORTED_PLATFORMS
                ENQUEUE_UNIQUE_RENDER_COMMAND(OnPause,
                        {
                                WVR_PauseATW();
                        });
                /*
		auto egl = AndroidEGL::GetInstance();
		if (!bNeedReinitEGL && egl->IsInitialized()) {
			egl->UnBind();
			bNeedReinitEGL = true;
		}
		*/
#endif
		if (CustomPresent != nullptr)
			CustomPresent->Reset();
	}
}

void FWaveVRHMD::OnResume()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	if (bResumed)
		return;
	bResumed = true;

#if WAVEVR_SUPPORTED_PLATFORMS
        ENQUEUE_UNIQUE_RENDER_COMMAND(OnResume,
                {
                        WVR_ResumeATW();
                });
        /*
	auto egl = AndroidEGL::GetInstance();
	if (bNeedReinitEGL && !egl->IsInitialized()) {
		egl->ReInit();
		bNeedReinitEGL = false;
	}
        */
#endif
}

void FWaveVRHMD::ApplicationWillEnterBackgroundDelegate()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	OnPause();
}

void FWaveVRHMD::ApplicationWillDeactivateDelegate()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	OnPause();
}

void FWaveVRHMD::ApplicationHasReactivatedDelegate()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	OnResume();
}

void FWaveVRHMD::ApplicationHasEnteredForegroundDelegate()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	OnResume();
}

void FWaveVRHMD::OnFEngineLoopInitComplete()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	setDeviceSupportedMaxFPS();
}

void FWaveVRHMD::Shutdown()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	if (bWVRInitialized == true)
	{
		if (CustomPresent != nullptr)
			CustomPresent->Shutdown();
		delete CustomPresent;
		CustomPresent = nullptr;

		if (DistortionMeshVerticesLeftEye != nullptr)
			delete[] DistortionMeshVerticesLeftEye;
		DistortionMeshVerticesLeftEye = nullptr;

		if (DistortionMeshVerticesRightEye != nullptr)
			delete[] DistortionMeshVerticesRightEye;
		DistortionMeshVerticesRightEye = nullptr;

		if (DistortionMeshIndices != nullptr)
			delete[] DistortionMeshIndices;
		DistortionMeshIndices = nullptr;

#if WAVEVR_SUPPORTED_PLATFORMS
		WVR_Quit();
#endif
	}
	bWVRInitialized = false;
	Instance = nullptr;
}

void FWaveVRHMD::ResetProjectionMats()
{
	LOG_FUNC();

    float Left, Right, Top, Bottom;
    //Left
    WVR_Eye hmdEye = WVR_Eye_Left;
#if WAVEVR_SUPPORTED_PLATFORMS
    WVR_GetClippingPlaneBoundary(hmdEye, &Right, &Left, &Bottom, &Top);

    Right *= -1.0f;
    Left*= -1.0f;
    Top *= -1.0f;
    Bottom *= -1.0f;
#else
	Right = Top = 1.0f;
	Left = Bottom = -1.0f;
#endif

    float ZNear = GNearClippingPlane;

    float SumRL = (Right + Left);
    float SumTB = (Top + Bottom);
    float InvRL = (1.0f / (Right - Left));
    float InvTB = (1.0f / (Top - Bottom));

    LeftProjection = new FMatrix(
            FPlane((2.0f * InvRL), 0.0f, 0.0f, 0.0f),
            FPlane(0.0f, (2.0f * InvTB), 0.0f, 0.0f),
            FPlane((SumRL * InvRL), (SumTB * InvTB), 0.0f, 1.0f),
            FPlane(0.0f, 0.0f, ZNear, 0.0f)
            );

    //Right
    hmdEye = WVR_Eye_Right;
#if WAVEVR_SUPPORTED_PLATFORMS
    WVR_GetClippingPlaneBoundary(hmdEye, &Right, &Left, &Bottom, &Top);

    Right *= -1.0f;
    Left*= -1.0f;
    Top *= -1.0f;
    Bottom *= -1.0f;
#else
	Right = Top = 1.0f;
	Left = Bottom = -1.0f;
#endif

    ZNear = GNearClippingPlane;

    SumRL = (Right + Left);
    SumTB = (Top + Bottom);
    InvRL = (1.0f / (Right - Left));
    InvTB = (1.0f / (Top - Bottom));

    RightProjection = new FMatrix(
            FPlane((2.0f * InvRL), 0.0f, 0.0f, 0.0f),
            FPlane(0.0f, (2.0f * InvTB), 0.0f, 0.0f),
            FPlane((SumRL * InvRL), (SumTB * InvTB), 0.0f, 1.0f),
            FPlane(0.0f, 0.0f, ZNear, 0.0f)
            );
}

 //------  Called on Game Thread ------
void FWaveVRHMD::SetupViewFamily(FSceneViewFamily& InViewFamily)
{
	LOG_FUNC();
	InViewFamily.EngineShowFlags.MotionBlur = 0;
	InViewFamily.EngineShowFlags.HMDDistortion = bUseUnrealDistortion;
	InViewFamily.EngineShowFlags.ScreenPercentage = false;
	InViewFamily.EngineShowFlags.StereoRendering = true;

	InViewFamily.EngineShowFlags.Lighting = true;
	InViewFamily.bResolveScene = true;
}


#if PLATFORM_ANDROID
extern "C" void Java_com_htc_vr_unreal_PermissionManager_initNative(JNIEnv* LocalJNIEnv, jobject LocalThiz) {
	LOG_FUNC();
	__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Java_com_epicgames_ue4_GameActivity_initNative");

	JavaENV = LocalJNIEnv;// FAndroidApplication::GetJavaEnv();

	JavaENV->GetJavaVM(&mJavaVM);

	if (mJavaVM == nullptr) {
		__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Java VM is null!!");
		return;
	}

	mJavaObj = JavaENV->NewGlobalRef(LocalThiz);

	jclass localClass = JavaENV->FindClass("com/htc/vr/unreal/PermissionManager");
	if (localClass == nullptr) {
		__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s: Can't find Java Class - com/htc/vr/unreal/PermissionManager.", __func__);
		return;
	}

	mJavaclazz = reinterpret_cast<jclass>(JavaENV->NewGlobalRef(localClass));
}

extern "C" void Java_com_htc_vr_unreal_PermissionManager_requestCallbackNative(JNIEnv* LocalJNIEnv, jobject LocalThiz, jobjectArray PermissionArray, jbooleanArray resultArray) {
	LOG_FUNC();
	int PermissionSize = LocalJNIEnv->GetArrayLength(PermissionArray);
    int ResultSize = LocalJNIEnv->GetArrayLength(resultArray);
    int ArraySize = (PermissionSize > ResultSize) ? ResultSize : PermissionSize;

    PermissionArr.Empty();
    ResultArr.Empty();
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "------ ArraySize = %d", ArraySize);
    jboolean* javaBarray = LocalJNIEnv->GetBooleanArrayElements(resultArray, 0);

    for (int i = 0; i < ArraySize; i++)
    {
        jstring string = (jstring)LocalJNIEnv->GetObjectArrayElement(PermissionArray, i);
        jboolean boolr = javaBarray[i];
        const char* myarray = LocalJNIEnv->GetStringUTFChars(string, 0);
        FString tmp = FString(UTF8_TO_TCHAR(myarray));
        PermissionArr.Add(tmp);
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Permission = %s", TCHAR_TO_ANSI(*PermissionArr[i]));
        ResultArr.Add(boolr);
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Result = %d", ResultArr[i]);
        LocalJNIEnv->ReleaseStringUTFChars(string, myarray);
        LocalJNIEnv->DeleteLocalRef(string);
    }
    URequestResultObject::onRequestResultNative.Broadcast(PermissionArr, ResultArr);
}
#endif
