// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "IWaveVRInputModule.h"
#include "WaveVRHMD.h"

#include "IInputInterface.h"
#include "IMotionController.h"
#include "IHapticDevice.h"

DEFINE_LOG_CATEGORY_STATIC(LogWaveVRInput, Log, All);

//-------------------------------------------------------------------------------------------------
// FWaveVRInput
//-------------------------------------------------------------------------------------------------

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

	// IInputDevice overrides
	virtual void Tick( float DeltaTime ) override;
	virtual void SendControllerEvents() override;
	virtual void SetMessageHandler( const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler ) override;
	virtual bool Exec( UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar ) override;
	virtual void SetChannelValue( int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value ) override;
	virtual void SetChannelValues( int32 ControllerId, const FForceFeedbackValues& Values ) override;

public:	// IMotionController overrides
	virtual FName GetMotionControllerDeviceTypeName() const override;
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
