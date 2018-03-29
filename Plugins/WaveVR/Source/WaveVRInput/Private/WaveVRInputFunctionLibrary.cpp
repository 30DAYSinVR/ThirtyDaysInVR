// Fill out your copyright notice in the Description page of Project Settings.

#include "WaveVRInput.h"
#include "Classes/WaveVRInputFunctionLibrary.h"


FWaveVRInput* GetWaveVRInput()
{
	TArray<IMotionController*> MotionControllers = IModularFeatures::Get().GetModularFeatureImplementations<IMotionController>(IMotionController::GetModularFeatureName());
	for (auto MotionController : MotionControllers)
	{
		if (MotionController != nullptr && MotionController->GetMotionControllerDeviceTypeName() == FWaveVRInput::DeviceTypeName)
		{
			return static_cast<FWaveVRInput*>(MotionController);
		}
	}

	return nullptr;
}

FRotator UWaveVRInputFunctionLibrary::GetWaveVRInputControllerOrientation(EControllerHand hand)
{
	FWaveVRInput* WaveVRInput = GetWaveVRInput();
	if (WaveVRInput != nullptr)
	{
		FRotator orientation;
		FVector position;
		bool bIsValid = WaveVRInput->GetControllerOrientationAndPosition(0, hand, orientation, position, 0);
		return bIsValid ? orientation : FRotator::ZeroRotator;
	}
	return FRotator::ZeroRotator;
}

FVector UWaveVRInputFunctionLibrary::GetWaveVRInputControllerPosition(EControllerHand hand)
{
	FWaveVRInput* WaveVRInput = GetWaveVRInput();
	if (WaveVRInput != nullptr)
	{
		FRotator orientation;
		FVector position;
		bool bIsValid = WaveVRInput->GetControllerOrientationAndPosition(0, hand, orientation, position, 0);
		return bIsValid ? position : FVector::ZeroVector;
	}
	return FVector::ZeroVector;
}

ETrackingStatus UWaveVRInputFunctionLibrary::GetWaveVRInputControllerTrackingStatus(EControllerHand hand)
{
	ETrackingStatus status = ETrackingStatus::NotTracked;

	FWaveVRInput* WaveVRInput = GetWaveVRInput();
	if (WaveVRInput != nullptr)
	{
		status = WaveVRInput->GetControllerTrackingStatus((int32)hand, hand);
	}

	return status;
}

FVector UWaveVRInputFunctionLibrary::GetWaveVRInputControllerVelocity(EControllerHand hand)
{
	FVector velocity = FVector::ZeroVector;

	switch (hand)
	{
	case EControllerHand::Right:
		velocity = UWaveVRBlueprintFunctionLibrary::GetDeviceVelocity(EWVR_DeviceType::DeviceType_Controller_Right);
		break;
	case EControllerHand::Left:
		velocity = UWaveVRBlueprintFunctionLibrary::GetDeviceVelocity(EWVR_DeviceType::DeviceType_Controller_Left);
		break;
	default:
		break;
	}

	return velocity;
}

FVector UWaveVRInputFunctionLibrary::GetWaveVRInputControllerAngularVelocity(EControllerHand hand)
{
	FVector angularv = FVector::ZeroVector;

	switch (hand)
	{
	case EControllerHand::Right:
		angularv = UWaveVRBlueprintFunctionLibrary::GetDeviceAngularVelocity(EWVR_DeviceType::DeviceType_Controller_Right);
		break;
	case EControllerHand::Left:
		angularv = UWaveVRBlueprintFunctionLibrary::GetDeviceAngularVelocity(EWVR_DeviceType::DeviceType_Controller_Left);
		break;
	default:
		break;
	}

	return angularv;
}

