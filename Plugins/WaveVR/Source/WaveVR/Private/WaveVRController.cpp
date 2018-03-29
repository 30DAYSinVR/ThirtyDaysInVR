// Fill out your copyright notice in the Description page of Project Settings.

#include "WaveVRPrivatePCH.h"
#include "WaveVRController.h"
#include "PoseManagerImp.h"

DEFINE_LOG_CATEGORY(LogWaveVRController);

#if WAVEVR_SUPPORTED_PLATFORMS
FWaveVRHMD* GetWaveVRHMD();
#endif

#pragma region Timer
bool Device::AllowPressActionInAFrame(EWVR_InputId id)
{
	for (unsigned int i = 0; i < InputButtonCount; i++)
	{
		if (id == InputButton[i])
		{
			milliseconds _ms = duration_cast< milliseconds >(
				system_clock::now().time_since_epoch()
				);
			if (_ms != prevFrameCount_press[i])
			{
				prevFrameCount_press[i] = _ms;
				return true;
			}
		}
	}

	return false;
}

bool Device::AllowTouchActionInAFrame(EWVR_TouchId id)
{
	for (unsigned int i = 0; i < TouchButtonCount; i++)
	{
		if (id == TouchButton[i])
		{
			milliseconds _ms = duration_cast< milliseconds >(
				system_clock::now().time_since_epoch()
				);
			if (_ms != prevFrameCount_touch[i])
			{
				prevFrameCount_touch[i] = _ms;
				return true;
			}
		}
	}

	return false;
}

bool Device::AllowGetAxisInAFrame(EWVR_TouchId id)
{
	for (unsigned int i = 0; i < TouchButtonCount; i++)
	{
		if (id == TouchButton[i])
		{
			milliseconds _ms = duration_cast< milliseconds >(
				system_clock::now().time_since_epoch()
				);
			if (_ms != prevFrameCount_axis[i])
			{
				prevFrameCount_axis[i] = _ms;
				return true;
			}
		}
	}

	return false;
}
#pragma endregion

#pragma region Class Device
Device::Device(EWVR_Hand hand)
	: axis_touchpad(FVector2D::ZeroVector), axis_trigger(FVector2D::ZeroVector)
{
	for (int i = 0; i < InputButtonCount; i++)
	{
		preState_press[i] = false;
		curState_press[i] = false;
		prevFrameCount_press[i] = { milliseconds(0) };
	}
	for (int i = 0; i < TouchButtonCount; i++)
	{
		preState_touch[i] = false;
		curState_touch[i] = false;
		prevFrameCount_touch[i] = { milliseconds(0) };
		prevFrameCount_axis[i] = { milliseconds(0) };
	}
	Hand = hand;
}

// -------------------- Press --------------------
void Device::Update_PressState(EWVR_InputId id)
{
	if (!AllowPressActionInAFrame(id))
		return;

	bool _pressed = false;

#if WAVEVR_SUPPORTED_PLATFORMS
	if (FWaveVRHMD::IsWVRInitialized()) {
		_pressed = WVR_GetInputButtonState((WVR_DeviceType)Hand, (WVR_InputId)id);
	}
#endif
	for (unsigned int i = 0; i < InputButtonCount; i++)
	{
		if (id == InputButton[i])
		{
			preState_press[i] = curState_press[i];
			curState_press[i] = _pressed;
			break;
		}
	}
}

bool Device::GetPress(EWVR_InputId id)
{
	bool _state = false;
	Update_PressState(id);

	for (unsigned int i = 0; i < InputButtonCount; i++)
	{
		if (id == InputButton[i])
		{
			_state = curState_press[i];
			break;
		}
	}

	return _state;
}

bool Device::GetPressDown(EWVR_InputId id)
{
	bool _state = false;
	Update_PressState(id);

	for (unsigned int i = 0; i < InputButtonCount; i++)
	{
		if (id == InputButton[i])
		{
			if (curState_press[i] == true && preState_press[i] == false)
			{
				_state = true;
			}
			break;
		}
	}

	return _state;
}

bool Device::GetPressUp(EWVR_InputId id)
{
	bool _state = false;
	Update_PressState(id);

	for (unsigned int i = 0; i < InputButtonCount; i++)
	{
		if (id == InputButton[i])
		{
			if (curState_press[i] == false && preState_press[i] == true)
			{
				_state = true;
			}
			break;
		}
	}

	return _state;
}

// -------------------- Touch --------------------
void Device::Update_TouchState(EWVR_TouchId id)
{
	if (EWVR_TouchId::Touchpad != id && EWVR_TouchId::Trigger != id)
	{
		LOGD(LogWaveVRController, "Update_TouchState() invalid id %d, need %d or %d", (uint8)id, (uint8)EWVR_TouchId::Touchpad, (uint8)EWVR_TouchId::Trigger);
		return;
	}
	if (!AllowTouchActionInAFrame(id))
		return;

	bool _touched = false;
#if WAVEVR_SUPPORTED_PLATFORMS
	if (FWaveVRHMD::IsWVRInitialized()) {
		_touched = WVR_GetInputTouchState((WVR_DeviceType)Hand, (WVR_InputId)id);
	}
#endif

	for (unsigned int i = 0; i < TouchButtonCount; i++)
	{
		if (id == TouchButton[i])
		{
			preState_touch[i] = curState_touch[i];
			curState_touch[i] = _touched;
			break;
		}
	}
}

bool Device::GetTouch(EWVR_TouchId id)
{
	bool _state = false;
	Update_TouchState(id);

	for (unsigned int i = 0; i < TouchButtonCount; i++)
	{
		if (id == TouchButton[i])
		{
			_state = curState_touch[i];
			break;
		}
	}

	return _state;
}

bool Device::GetTouchDown(EWVR_TouchId id)
{
	bool _state = false;
	Update_TouchState(id);

	for (unsigned int i = 0; i < TouchButtonCount; i++)
	{
		if (id == TouchButton[i])
		{
			if (curState_touch[i] == true && preState_touch[i] == false)
			{
				_state = true;
			}
			break;
		}
	}

	return _state;
}

bool Device::GetTouchUp(EWVR_TouchId id)
{
	bool _state = false;
	Update_TouchState(id);

	for (unsigned int i = 0; i < TouchButtonCount; i++)
	{
		if (id == TouchButton[i])
		{
			if (curState_touch[i] == false && preState_touch[i] == true)
			{
				_state = true;
			}
			break;
		}
	}

	return _state;
}

FVector2D Device::GetAxis(EWVR_TouchId id)
{
	bool _allow = false;
	WVR_Axis_t _axis = {0, 0};
	FVector2D _vec = FVector2D::ZeroVector;

	if (AllowGetAxisInAFrame(id))
	{
#if WAVEVR_SUPPORTED_PLATFORMS
		if (FWaveVRHMD::IsWVRInitialized()) {
			_axis = WVR_GetInputAnalogAxis((WVR_DeviceType)Hand, (WVR_InputId)id);
			LOGD(LogWaveVRController, "GetAxis(), axis:{%f, %f}", _axis.x, _axis.y);
		}
#endif
		_allow = true;
	}

	switch (id)
	{
	case EWVR_TouchId::Touchpad:
		if (_allow)
		{
			axis_touchpad.X = _axis.x;
			axis_touchpad.Y = _axis.y;
		}
		// If updating axis is now allowed, use previous axis.
		_vec = CoordinatUtil::getVector2(axis_touchpad);
		break;
	case EWVR_TouchId::Trigger:
		if (_allow)
		{
			axis_trigger.X = _axis.x;
		}
		// If updating axis is now allowed, use previous axis.
		_vec = CoordinatUtil::getVector2(axis_trigger);
		break;
	default:
		break;
	}

	return _vec;
}
#pragma endregion

#pragma region Class UWaveVRController
UWaveVRController::UWaveVRController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

Device UWaveVRController::RightController(EWVR_Hand::Hand_Controller_Right);
Device UWaveVRController::LeftController(EWVR_Hand::Hand_Controller_Left);

bool UWaveVRController::IsRightControllerButtonPressed(EWVR_InputId button_id)
{
	bool _pressed = UWaveVRController::RightController.GetPress(button_id);

	return _pressed;
}

bool UWaveVRController::IsLeftControllerButtonPressed(EWVR_InputId button_id)
{
	bool _pressed = UWaveVRController::LeftController.GetPress(button_id);

	return _pressed;
}

bool UWaveVRController::IsRightControllerButtonTouched(EWVR_TouchId button_id)
{
	bool _touched = UWaveVRController::RightController.GetTouch(button_id);

	return _touched;
}

bool UWaveVRController::IsLeftControllerButtonTouched(EWVR_TouchId button_id)
{
	bool _touched = UWaveVRController::LeftController.GetTouch(button_id);

	return _touched;
}

bool UWaveVRController::IsRightControllerConnected()
{
	bool _connected = false;

#if WAVEVR_SUPPORTED_PLATFORMS
	if (FWaveVRHMD::IsWVRInitialized()) {
		if (WVR_IsDeviceConnected(WVR_DeviceType_Controller_Right))
			_connected = true;
	}
#endif

	return _connected;
}

bool UWaveVRController::IsLeftControllerConnected()
{
	bool _connected = false;

#if WAVEVR_SUPPORTED_PLATFORMS
	if (FWaveVRHMD::IsWVRInitialized()) {
		if (WVR_IsDeviceConnected(WVR_DeviceType_Controller_Left))
			_connected = true;
	}
#endif

	return _connected;
}

bool UWaveVRController::IsAnyButtonPressed()
{
	bool _pressed = false;

	if (IsRightControllerConnected())
	{
		for (unsigned int i = 0; i < InputButtonCount; i++) {
			EWVR_InputId button_id = InputButton[i];
			_pressed = IsRightControllerButtonPressed(button_id);
			if (_pressed)
			{
				LOGD(LogWaveVRController, "IsAnyButtonPressed() right %d is pressed", (uint8)button_id);
				break;
			}
		}
	}
	else if (IsLeftControllerConnected())
	{
		for (unsigned int i = 0; i < (sizeof(InputButton) / sizeof(*InputButton)); i++) {
			EWVR_InputId button_id = InputButton[i];
			_pressed = IsLeftControllerButtonPressed(button_id);
			if (_pressed)
			{
				LOGD(LogWaveVRController, "IsAnyButtonPressed() left %d is pressed", (uint8)button_id);
				break;
			}
		}
	}

	return _pressed;
}

FVector2D UWaveVRController::GetRightControllerAxis(EWVR_TouchId button_id)
{
	FVector2D _vec = UWaveVRController::RightController.GetAxis(button_id);

	return _vec;
}

FVector2D UWaveVRController::GetLeftControllerAxis(EWVR_TouchId button_id)
{
	FVector2D _vec = UWaveVRController::LeftController.GetAxis(button_id);

	return _vec;
}
/*
bool UWaveVRController::IsRightControllerButtonPressedDown(EWVR_InputId button_id)
{
	bool _state = UWaveVRController::RightController.GetPressDown(button_id);

	return _state;
}

bool UWaveVRController::IsRightControllerButtonPressedUp(EWVR_InputId button_id)
{
	bool _state = UWaveVRController::RightController.GetPressUp(button_id);

	return _state;
}

bool UWaveVRController::IsLeftControllerButtonPressedDown(EWVR_InputId button_id)
{
	bool _state = UWaveVRController::LeftController.GetPressDown(button_id);

	return _state;
}

bool UWaveVRController::IsLeftControllerButtonPressedUp(EWVR_InputId button_id)
{
	bool _state = UWaveVRController::LeftController.GetPressUp(button_id);

	return _state;
}

bool UWaveVRController::IsRightControllerButtonTouchedDown(EWVR_TouchId button_id)
{
	bool _touched = UWaveVRController::RightController.GetTouchDown(button_id);

	return _touched;
}

bool UWaveVRController::IsRightControllerButtonTouchedUp(EWVR_TouchId button_id)
{
	bool _touched = UWaveVRController::RightController.GetTouchUp(button_id);

	return _touched;
}

bool UWaveVRController::IsLeftControllerButtonTouchedDown(EWVR_TouchId button_id)
{
	bool _touched = UWaveVRController::LeftController.GetTouchDown(button_id);

	return _touched;
}

bool UWaveVRController::IsLeftControllerButtonTouchedUp(EWVR_TouchId button_id)
{
	bool _touched = UWaveVRController::LeftController.GetTouchUp(button_id);

	return _touched;
}
*/
#pragma endregion
