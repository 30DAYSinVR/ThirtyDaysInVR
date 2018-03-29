// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "IWaveVRInputModule.h"
#include "WaveVRHMD.h"
#include "WaveVRController.h"

#include "IInputInterface.h"
#include "IMotionController.h"
#include "IHapticDevice.h"
#if WAVEVR_LOG_ANDROID_LOG
#include <android/Log.h>
#endif

DEFINE_LOG_CATEGORY_STATIC(LogWaveVRInput, Log, All);

//-------------------------------------------------------------------------------------------------
// FWaveVRInput
//-------------------------------------------------------------------------------------------------

namespace WaveVRControllerKeyNames
{
	const FGamepadKeyNames::Type AndroidVolumeUp	("Android_Volume_Up");
	const FGamepadKeyNames::Type AndroidVolumeDown	("Android_Volume_Down");

	const FGamepadKeyNames::Type LeftBumper			("Gamepad_Special_Left");
	const FGamepadKeyNames::Type RightBumper		("Gamepad_Special_Right");

	const FGamepadKeyNames::Type LeftTouchpadTouch	("Steam_Touch_0");
	const FGamepadKeyNames::Type RightTouchpadTouch	("Steam_Touch_1");
	const FGamepadKeyNames::Type LeftTriggerTouch	("Steam_Touch_2");
	const FGamepadKeyNames::Type RightTriggerTouch	("Steam_Touch_3");
}


class FWaveVRInput : public IInputDevice, public IMotionController, public IHapticDevice
{

       FWaveVRHMD* GetWaveVRHMD() const {
            if (GEngine->HMDDevice.IsValid() && (GEngine->HMDDevice->GetDeviceName() == TEXT("WaveVR")))
            {
                UE_LOG(LogWaveVRInput, Log, TEXT("GetWaveVRHMD"));
                return static_cast<FWaveVRHMD*>(GEngine->HMDDevice.Get());
            }
            return nullptr;
        }
public:

	/** Constructor that takes an initial message handler that will receive motion controller events */
	FWaveVRInput( const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler );

	/** Clean everything up */
	virtual ~FWaveVRInput();

	static const int ControllerCount = 2;
	FGamepadKeyNames::Type ControllerPressButtons[ControllerCount][InputButtonCount];
	bool PrevPressState[ControllerCount][InputButtonCount];
	FGamepadKeyNames::Type ControllerTouchButtons[ControllerCount][TouchButtonCount];
	bool PrevTouchState[ControllerCount][TouchButtonCount];

public:	// IInputDevice overrides
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override
	{
		return false;
	}
	virtual void SendControllerEvents() override;
	/**
	 * IForceFeedbackSystem pass through functions
	 * This class is deprecated and will be removed in favor of IInputInterface
	 */
	virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override {}
	virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues& Values) override {}

	virtual void SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) override
	{
		MessageHandler = InMessageHandler;
	}
	virtual void Tick(float DeltaTime) override
	{
		// Do nothing since WaveVR updates device pose every frame and get device connection & button states passively.
	}

private:
	bool IsControllerConnected(EControllerHand hand);
	void UpdateButtonPressStates(EControllerHand hand);
	void UpdateButtonTouchStates(EControllerHand hand);

public:	// IMotionController overrides
	static FName DeviceTypeName;
	virtual FName GetMotionControllerDeviceTypeName() const override
	{
		return DeviceTypeName;
	}
	virtual bool GetControllerOrientationAndPosition( const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition, float WorldToMetersScale ) const override;
	virtual ETrackingStatus GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const override;

public:	// IHapticDevice overrides
	IHapticDevice* GetHapticDevice() override { return (IHapticDevice*)this; }
	virtual void SetHapticFeedbackValues(int32 ControllerId, int32 Hand, const FHapticFeedbackValues& Values) override;

	virtual void GetHapticFrequencyRange(float& MinFrequency, float& MaxFrequency) const override;
	virtual float GetHapticAmplitudeScale() const override;

private:

        //FWaveVRHMD* HmdPlugin = FWaveVRHMD::GetInstance();

	/** The recipient of motion controller input events */
	TSharedPtr< FGenericApplicationMessageHandler > MessageHandler;
};
