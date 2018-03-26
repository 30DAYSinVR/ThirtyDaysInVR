// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "WaveVRInput.h"
#include "Features/IModularFeatures.h"
#include "WaveVRBlueprintFunctionLibrary.h"

FWaveVRInput::FWaveVRInput( const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler )
	: MessageHandler( InMessageHandler )
{
    LOG_FUNC();
    IModularFeatures::Get().RegisterModularFeature( GetModularFeatureName(), this );
    UE_LOG(LogWaveVRInput, Log, TEXT("WaveVRInput is initialized"));
}


FWaveVRInput::~FWaveVRInput()
{
    LOG_FUNC();
    IModularFeatures::Get().UnregisterModularFeature( GetModularFeatureName(), this );
}


void FWaveVRInput::Tick( float DeltaTime )
{
    LOG_FUNC();
}


void FWaveVRInput::SendControllerEvents()
{
    LOG_FUNC();
    //TODO
}


void FWaveVRInput::SetMessageHandler( const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler )
{
    LOG_FUNC();
    //TODO
#if 0
    MessageHandler = InMessageHandler;
#endif
}


bool FWaveVRInput::Exec( UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar )
{
    LOG_FUNC();
    // No exec commands supported, for now.
    return false;
}

void FWaveVRInput::SetChannelValue( int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value )
{
    LOG_FUNC();
    //TODO
}

void FWaveVRInput::SetChannelValues( int32 ControllerId, const FForceFeedbackValues& Values )
{
    LOG_FUNC();
    //TODO
}

FName FWaveVRInput::GetMotionControllerDeviceTypeName() const
{
    LOG_FUNC();
    const static FName DefaultName(TEXT("WaveVRInputDevice"));
    return DefaultName;
}

bool FWaveVRInput::GetControllerOrientationAndPosition( const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition, float WorldToMetersScale) const
{
    LOG_FUNC();
    bool bIsValid = false;
    EWVR_DeviceType type = EWVR_DeviceType::DeviceType_Controller_Right;
    if(DeviceHand == EControllerHand::Left) {
        type = EWVR_DeviceType::DeviceType_Controller_Left;
    } else if (DeviceHand == EControllerHand::Right){
        type = EWVR_DeviceType::DeviceType_Controller_Right;
    }
    bIsValid = UWaveVRBlueprintFunctionLibrary::GetDevicePose(OutPosition, OutOrientation, type);
    UE_LOG(LogHMD, Log, TEXT("Position(X,Y,Z) is (%f, %f, %f)"), OutPosition.X, OutPosition.Y, OutPosition.Z);
    UE_LOG(LogHMD, Log, TEXT("Rotator(Pitch,Roll,Yaw) is(%f, %f, %f)"), OutOrientation.Pitch, OutOrientation.Roll, OutOrientation.Yaw);
    return bIsValid;
}

ETrackingStatus FWaveVRInput::GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const
{
    LOG_FUNC();
    //TODO
    return ETrackingStatus::NotTracked;
}

void FWaveVRInput::SetHapticFeedbackValues(int32 ControllerId, int32 Hand, const FHapticFeedbackValues& Values)
{
    LOG_FUNC();
}

void FWaveVRInput::GetHapticFrequencyRange(float& MinFrequency, float& MaxFrequency) const
{
    LOG_FUNC();
    MinFrequency = 0.0f;
    MaxFrequency = 1.0f;
}

float FWaveVRInput::GetHapticAmplitudeScale() const
{
    LOG_FUNC();
    return 1.0f;
}
