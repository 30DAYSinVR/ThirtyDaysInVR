// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "WaveVRInput.h"
#include "Features/IModularFeatures.h"
#include "WaveVRBlueprintFunctionLibrary.h"

static bool IsPlayInEditor()
{
	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		if (Context.World()->IsPlayInEditor())
		{
			return true;
		}
	}
	return false;
}


FName FWaveVRInput::DeviceTypeName(TEXT("WaveVRInput"));

FWaveVRInput::FWaveVRInput( const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler )
	: MessageHandler( InMessageHandler )
{
    LOG_FUNC();
    IModularFeatures::Get().RegisterModularFeature( GetModularFeatureName(), this );

	for (int i = 0; i < ControllerCount; i++)
	{
		for (int j = 0; j < InputButtonCount; j++)
			PrevPressState[i][j] = false;
		for (int j = 0; j < TouchButtonCount; j++)
			PrevTouchState[i][j] = false;
	}

	int32 right_hand = (int32)EControllerHand::Right;
	int32 left_hand = (int32)EControllerHand::Left;

	// ----------------- Press Buttons --------------------
	// EWVR_InputId::Menu
	ControllerPressButtons[right_hand][0] = FGamepadKeyNames::MotionController_Right_Shoulder;
	ControllerPressButtons[left_hand][0] = FGamepadKeyNames::MotionController_Left_Shoulder;
	// EWVR_InputId::Grip
	ControllerPressButtons[right_hand][1] = FGamepadKeyNames::MotionController_Right_Grip1;
	ControllerPressButtons[left_hand][1] = FGamepadKeyNames::MotionController_Left_Grip1;
	// EWVR_InputId::DPad_Left
	ControllerPressButtons[right_hand][2] = FGamepadKeyNames::MotionController_Right_FaceButton4;
	ControllerPressButtons[left_hand][2] = FGamepadKeyNames::MotionController_Left_FaceButton4;
	// EWVR_InputId::DPad_Up
	ControllerPressButtons[right_hand][3] = FGamepadKeyNames::MotionController_Right_FaceButton1;
	ControllerPressButtons[left_hand][3] = FGamepadKeyNames::MotionController_Left_FaceButton1;
	// EWVR_InputId::DPad_Right
	ControllerPressButtons[right_hand][4] = FGamepadKeyNames::MotionController_Right_FaceButton2;
	ControllerPressButtons[left_hand][4] = FGamepadKeyNames::MotionController_Left_FaceButton2;
	// EWVR_InputId::DPad_Down
	ControllerPressButtons[right_hand][5] = FGamepadKeyNames::MotionController_Right_FaceButton3;
	ControllerPressButtons[left_hand][5] = FGamepadKeyNames::MotionController_Left_FaceButton3;
	// EWVR_InputId::Volume_Up
	ControllerPressButtons[right_hand][6] = WaveVRControllerKeyNames::AndroidVolumeUp;
	ControllerPressButtons[left_hand][6] = WaveVRControllerKeyNames::AndroidVolumeUp;
	// EWVR_InputId::Volume_Down
	ControllerPressButtons[right_hand][7] = WaveVRControllerKeyNames::AndroidVolumeDown;
	ControllerPressButtons[left_hand][7] = WaveVRControllerKeyNames::AndroidVolumeDown;
	// EWVR_InputId::Bumper
	ControllerPressButtons[right_hand][8] = WaveVRControllerKeyNames::RightBumper;
	ControllerPressButtons[left_hand][8] = WaveVRControllerKeyNames::LeftBumper;
	// EWVR_InputId::Touchpad
	ControllerPressButtons[right_hand][9] = FGamepadKeyNames::MotionController_Right_Thumbstick;
	ControllerPressButtons[left_hand][9] = FGamepadKeyNames::MotionController_Left_Thumbstick;
	// EWVR_InputId::Trigger
	ControllerPressButtons[right_hand][10] = FGamepadKeyNames::MotionController_Right_Trigger;
	ControllerPressButtons[left_hand][10] = FGamepadKeyNames::MotionController_Left_Trigger;

	// -------------- Touch Buttons ----------------
	// EWVR_InputId::Touchpad
	ControllerTouchButtons[right_hand][0] = WaveVRControllerKeyNames::RightTouchpadTouch;
	ControllerTouchButtons[left_hand][0] = WaveVRControllerKeyNames::LeftTouchpadTouch;
	// EWVR_InputId::Trigger
	ControllerTouchButtons[right_hand][1] = WaveVRControllerKeyNames::RightTriggerTouch;
	ControllerTouchButtons[left_hand][1] = WaveVRControllerKeyNames::LeftTriggerTouch;


	LOG_NOARG(LogWaveVRInput, "WaveVRInput is initialized");
}


FWaveVRInput::~FWaveVRInput()
{
    LOG_FUNC();
    IModularFeatures::Get().UnregisterModularFeature( GetModularFeatureName(), this );
}

bool FWaveVRInput::IsControllerConnected(EControllerHand hand)
{
	bool _connected = false;
	switch (hand)
	{
	case EControllerHand::Right:
		_connected = UWaveVRController::IsRightControllerConnected();
		break;
	case EControllerHand::Left:
		_connected = UWaveVRController::IsLeftControllerConnected();
		break;
	default:
		break;
	}
	return _connected;
}

void FWaveVRInput::UpdateButtonPressStates(EControllerHand hand)
{
	bool _connected = IsControllerConnected(hand);
	if (!_connected)
		return;

	int32 _hand = (int32)hand;
	bool _curPressState[InputButtonCount] = { false };

	// Get current buttons' states.
	for (int i = 0; i < InputButtonCount; i++)
	{
		EWVR_InputId inputBtn = InputButton[i];
		_curPressState[i] = UWaveVRController::IsRightControllerButtonPressed(inputBtn);
	}

	// Compare with previous buttons' states.
	for (int i = 0; i < InputButtonCount; i++)
	{
		FName button_name = ControllerPressButtons[_hand][i];

		if (_curPressState[i] != PrevPressState[_hand][i])
		{
			if (_curPressState[i])
			{
#if WAVEVR_LOG_ANDROID_LOG
				LOGD(LogWaveVRInput, "SendControllerEvents() hand %d button %s is pressed.", _hand, TCHAR_TO_ANSI(*button_name.ToString()));
#else
				UE_LOG(LogWaveVRInput, Display, TEXT("SendControllerEvents() hand %d button %s is pressed."), _hand, *button_name.ToString());
#endif
				MessageHandler->OnControllerButtonPressed(button_name, 0, false);
			}
			else
			{
#if WAVEVR_LOG_ANDROID_LOG
				LOGD(LogWaveVRInput, "SendControllerEvents() hand %d button %s is released.", _hand, TCHAR_TO_ANSI(*button_name.ToString()));
#else
				UE_LOG(LogWaveVRInput, Display, TEXT("SendControllerEvents() hand %d button %s is unpressed."), _hand, *button_name.ToString());
#endif
				MessageHandler->OnControllerButtonReleased(button_name, 0, false);
			}
		}

		PrevPressState[_hand][i] = _curPressState[i];
	}
}

void FWaveVRInput::UpdateButtonTouchStates(EControllerHand hand)
{
	bool _connected = IsControllerConnected(hand);
	if (!_connected)
		return;

	int32 _hand = (int32)hand;
	bool _curTouchState[TouchButtonCount] = { false };

	for (int i = 0; i < TouchButtonCount; i++)
	{
		EWVR_TouchId touchBtn = TouchButton[i];
		_curTouchState[i] = UWaveVRController::IsRightControllerButtonTouched(touchBtn);

		if (_curTouchState[i])
		{
			EWVR_TouchId _touchBtn = TouchButton[i];
			FVector2D axis = UWaveVRController::GetRightControllerAxis(touchBtn);
			switch (touchBtn)
			{
			case EWVR_TouchId::Touchpad:
				MessageHandler->OnControllerAnalog(FGamepadKeyNames::MotionController_Right_Thumbstick_X, 0, axis.X);
				MessageHandler->OnControllerAnalog(FGamepadKeyNames::MotionController_Right_Thumbstick_Y, 0, axis.Y);
				break;
			case EWVR_TouchId::Trigger:
				MessageHandler->OnControllerAnalog(FGamepadKeyNames::MotionController_Right_TriggerAxis, 0, axis.X);
				break;
			default:
				break;
			}
		}
	}

	for (int i = 0; i < TouchButtonCount; i++)
	{
		FName button_name = ControllerTouchButtons[_hand][i];

		if (_curTouchState[i] != PrevTouchState[_hand][i])
		{
			if (_curTouchState[i])
			{
#if WAVEVR_LOG_ANDROID_LOG
				LOGD(LogWaveVRInput, "SendControllerEvents() hand %d button %s is touched.", _hand, TCHAR_TO_ANSI(*button_name.ToString()));
#else
				UE_LOG(LogWaveVRInput, Display, TEXT("SendControllerEvents() hand %d button %s is touched."), _hand, *button_name.ToString());
#endif
				MessageHandler->OnControllerButtonPressed(button_name, 0, false);
			}
			else
			{
#if WAVEVR_LOG_ANDROID_LOG
				LOGD(LogWaveVRInput, "SendControllerEvents() hand %d button %s is released.", _hand, TCHAR_TO_ANSI(*button_name.ToString()));
#else
				UE_LOG(LogWaveVRInput, Display, TEXT("SendControllerEvents() hand %d button %s is untouched."), _hand, *button_name.ToString());
#endif
				MessageHandler->OnControllerButtonReleased(button_name, 0, false);
			}
		}

		PrevTouchState[_hand][i] = _curTouchState[i];
	}
}

#pragma region
void FWaveVRInput::SendControllerEvents()
{
    //LOGD(LogWaveVRInput, "SendControllerEvents()");
    
	UpdateButtonPressStates(EControllerHand::Right);
	UpdateButtonPressStates(EControllerHand::Left);
	UpdateButtonTouchStates(EControllerHand::Right);
	UpdateButtonTouchStates(EControllerHand::Left);
}
#pragma endregion IInputDevice overrides

#pragma region
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
    //UE_LOG(LogHMD, Log, TEXT("Position(X,Y,Z) is (%f, %f, %f)"), OutPosition.X, OutPosition.Y, OutPosition.Z);
    //UE_LOG(LogHMD, Log, TEXT("Rotator(Pitch,Roll,Yaw) is(%f, %f, %f)"), OutOrientation.Pitch, OutOrientation.Roll, OutOrientation.Yaw);
    return bIsValid;
}

ETrackingStatus FWaveVRInput::GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const
{
    LOG_FUNC();

	ETrackingStatus status = ETrackingStatus::NotTracked;

	switch (DeviceHand)
	{
	case EControllerHand::Right:
		if (UWaveVRController::IsRightControllerConnected())
			status = ETrackingStatus::Tracked;
		break;
	case EControllerHand::Left:
		if (UWaveVRController::IsLeftControllerConnected())
			status = ETrackingStatus::Tracked;
		break;
	case EControllerHand::AnyHand:
		if (UWaveVRController::IsRightControllerConnected() || UWaveVRController::IsLeftControllerConnected())
			status = ETrackingStatus::Tracked;
		break;
	default:
		break;
	}
    
    return status;
}
#pragma endregion IMotionController overrides

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

