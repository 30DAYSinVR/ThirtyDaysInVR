// Fill out your copyright notice in the Description page of Project Settings.

#include "WaveVRPrivatePCH.h"
#include "WaveVRController.h"
#include "PoseManagerImp.h"

DEFINE_LOG_CATEGORY(WaveVRController);

#if WAVEVR_SUPPORTED_PLATFORMS
FWaveVRHMD* GetWaveVRHMD();
#endif

#pragma region Timer
#if WAVEVR_SUPPORTED_PLATFORMS
bool Device::AllowPressActionInAFrame(WVR_InputId id)
{
	for (unsigned int i = 0; i < pressIdCount; i++)
	{
		if (id == pressIds[i])
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

bool Device::AllowTouchActionInAFrame(WVR_InputId id)
{
	for (unsigned int i = 0; i < touchIdCount; i++)
	{
		if (id == touchIds[i])
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
#endif
#pragma endregion

#pragma region Class Device
#if WAVEVR_SUPPORTED_PLATFORMS
Device::Device(WVR_DeviceType type)
	: axis_touchpad(FVector2D::ZeroVector), axis_trigger(FVector2D::ZeroVector)
{
	for (int i = 0; i < pressIdCount; i++)
	{
		preState_press[i] = false;
		curState_press[i] = false;
		prevFrameCount_press[i] = { milliseconds(0) };
	}
	for (int i = 0; i < touchIdCount; i++)
	{
		preState_touch[i] = false;
		curState_touch[i] = false;
		prevFrameCount_touch[i] = { milliseconds(0) };
	}
	DeviceType = type;
}

void Device::Update_PressState(WVR_InputId id)
{
	if (!AllowPressActionInAFrame(id))
		return;

	bool _pressed = WVR_GetInputButtonState(DeviceType, id);

	for (unsigned int i = 0; i < pressIdCount; i++)
	{
		if (id == pressIds[i])
		{
			preState_press[i] = curState_press[i];
			curState_press[i] = _pressed;
			break;
		}
	}
}

bool Device::GetPress(WVR_InputId id)
{
	bool _state = false;
	Update_PressState(id);

	for (unsigned int i = 0; i < pressIdCount; i++)
	{
		if (id == pressIds[i])
		{
			_state = curState_press[i];
			break;
		}
	}

	return _state;
}

bool Device::GetPressDown(WVR_InputId id)
{
	bool _state = false;
	Update_PressState(id);

	for (unsigned int i = 0; i < pressIdCount; i++)
	{
		if (id == pressIds[i])
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

bool Device::GetPressUp(WVR_InputId id)
{
	bool _state = false;
	Update_PressState(id);

	for (unsigned int i = 0; i < pressIdCount; i++)
	{
		if (id == pressIds[i])
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

void Device::Update_TouchState(WVR_InputId id)
{
	if (WVR_InputId_Alias1_Touchpad != id && WVR_InputId_Alias1_Trigger != id)
	{
		UE_LOG(WaveVRController, Warning, TEXT("Update_TouchState() invalid id %d, need %d or %d"), unsigned(id), WVR_InputId_Alias1_Touchpad, WVR_InputId_Alias1_Trigger);
		return;
	}
	if (!AllowTouchActionInAFrame(id))
		return;

	bool _touched = WVR_GetInputTouchState(DeviceType, id);

	for (unsigned int i = 0; i < touchIdCount; i++)
	{
		if (id == touchIds[i])
		{
			preState_touch[i] = curState_touch[i];
			curState_touch[i] = _touched;
			break;
		}
	}
}

bool Device::GetTouch(WVR_InputId id)
{
	bool _state = false;
	Update_TouchState(id);

	for (unsigned int i = 0; i < touchIdCount; i++)
	{
		if (id == touchIds[i])
		{
			_state = curState_touch[i];
			break;
		}
	}

	return _state;
}

bool Device::GetTouchDown(WVR_InputId id)
{
	bool _state = false;
	Update_TouchState(id);

	for (unsigned int i = 0; i < touchIdCount; i++)
	{
		if (id == touchIds[i])
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

bool Device::GetTouchUp(WVR_InputId id)
{
	bool _state = false;
	Update_TouchState(id);

	for (unsigned int i = 0; i < touchIdCount; i++)
	{
		if (id == touchIds[i])
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

FVector2D Device::GetAxis(WVR_InputId id)
{
	bool _allow = false;
	WVR_Axis_t _axis = {0, 0};
	FVector2D _vec = FVector2D::ZeroVector;

	if (AllowTouchActionInAFrame(id))
	{
		_axis = WVR_GetInputAnalogAxis(DeviceType, id);
		_allow = true;
	}

	switch (id)
	{
	case WVR_InputId_Alias1_Touchpad:
		if (_allow)
		{
			axis_touchpad.X = _axis.x;
			axis_touchpad.Y = _axis.y;
		}
		_vec = CoordinatUtil::getVector2(axis_touchpad);
		break;
	case WVR_InputId_Alias1_Trigger:
		if (_allow)
		{
			axis_trigger.X = _axis.x;
		}
		_vec = CoordinatUtil::getVector2(axis_trigger);
		break;
	default:
		break;
	}

	return _vec;
}
#endif
#pragma endregion

#pragma region Class UWaveVRController
UWaveVRController::UWaveVRController()
{
}

#if WAVEVR_SUPPORTED_PLATFORMS
Device UWaveVRController::RightController(WVR_DeviceType_Controller_Right);
Device UWaveVRController::LeftController(WVR_DeviceType_Controller_Left);
#endif

bool UWaveVRController::IsRightControllerButtonPressed(EWVR_InputId button_id)
{
	bool _pressed = false;
#if WAVEVR_SUPPORTED_PLATFORMS
	if (FWaveVRHMD::IsWVRInitialized()) {
		_pressed = UWaveVRController::RightController.GetPress((WVR_InputId)button_id);
	}
#endif
	return _pressed;
}

bool UWaveVRController::IsRightControllerButtonPressedDown(EWVR_InputId button_id)
{
	bool _state = false;
#if WAVEVR_SUPPORTED_PLATFORMS
	if (FWaveVRHMD::IsWVRInitialized()) {
		_state = UWaveVRController::RightController.GetPressDown((WVR_InputId)button_id);
	}
#endif
	return _state;
}

bool UWaveVRController::IsRightControllerButtonPressedUp(EWVR_InputId button_id)
{
	bool _state = false;
#if WAVEVR_SUPPORTED_PLATFORMS
	if (FWaveVRHMD::IsWVRInitialized()) {
		_state = UWaveVRController::RightController.GetPressUp((WVR_InputId)button_id);
	}
#endif
	return _state;
}

bool UWaveVRController::IsLeftControllerButtonPressed(EWVR_InputId button_id)
{
	bool _pressed = false;
#if WAVEVR_SUPPORTED_PLATFORMS
	if (FWaveVRHMD::IsWVRInitialized()) {
		_pressed = UWaveVRController::LeftController.GetPress((WVR_InputId)button_id);
	}
#endif
	return _pressed;
}

bool UWaveVRController::IsLeftControllerButtonPressedDown(EWVR_InputId button_id)
{
	bool _state = false;
#if WAVEVR_SUPPORTED_PLATFORMS
	if (FWaveVRHMD::IsWVRInitialized()) {
		_state = UWaveVRController::LeftController.GetPressDown((WVR_InputId)button_id);
	}
#endif
	return _state;
}

bool UWaveVRController::IsLeftControllerButtonPressedUp(EWVR_InputId button_id)
{
	bool _state = false;
#if WAVEVR_SUPPORTED_PLATFORMS
	if (FWaveVRHMD::IsWVRInitialized()) {
		_state = UWaveVRController::LeftController.GetPressUp((WVR_InputId)button_id);
	}
#endif
	return _state;
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

EWVR_InputId EButtonIds[11] = {
	EWVR_InputId::Menu,
	EWVR_InputId::Grip,
	EWVR_InputId::DPad_Left,
	EWVR_InputId::DPad_Up,
	EWVR_InputId::DPad_Right,	// 5
	EWVR_InputId::DPad_Down,
	EWVR_InputId::Volume_Up,
	EWVR_InputId::Volume_Down,
	EWVR_InputId::Bumper,
	EWVR_InputId::Touchpad,		// 10
	EWVR_InputId::Trigger
};


bool UWaveVRController::IsAnyButtonPressed()
{
	bool _pressed = false;

	if (IsRightControllerConnected())
	{
		for (unsigned int i = 0; i < (sizeof(EButtonIds) / sizeof(*EButtonIds)); i++) {
			EWVR_InputId button_id = EButtonIds[i];
			_pressed = IsRightControllerButtonPressed(button_id);
			if (_pressed)
			{
				UE_LOG(WaveVRController, Warning, TEXT("IsAnyButtonPressed() right %d is pressed"), unsigned(button_id));
				break;
			}
		}
	}
	else if (IsLeftControllerConnected())
	{
		for (unsigned int i = 0; i < (sizeof(EButtonIds) / sizeof(*EButtonIds)); i++) {
			EWVR_InputId button_id = EButtonIds[i];
			_pressed = IsLeftControllerButtonPressed(button_id);
			if (_pressed)
			{
				UE_LOG(WaveVRController, Warning, TEXT("IsAnyButtonPressed() left %d is pressed"), unsigned(button_id));
				break;
			}
		}
	}

	return _pressed;
}

bool UWaveVRController::IsRightControllerButtonTouched(EWVR_TouchId button_id)
{
	bool _touched = false;
#if WAVEVR_SUPPORTED_PLATFORMS
	if (FWaveVRHMD::IsWVRInitialized()) {
		switch (button_id)
		{
		case EWVR_TouchId::Touchpad:
			_touched = UWaveVRController::RightController.GetTouch(WVR_InputId_Alias1_Touchpad);
			break;
		case EWVR_TouchId::Trigger:
			_touched = UWaveVRController::RightController.GetTouch(WVR_InputId_Alias1_Trigger);
			break;
		default:
			break;
		}
	}
#endif
	return _touched;
}

bool UWaveVRController::IsRightControllerButtonTouchedDown(EWVR_TouchId button_id)
{
	bool _touched = false;
#if WAVEVR_SUPPORTED_PLATFORMS
	if (FWaveVRHMD::IsWVRInitialized()) {
		switch (button_id)
		{
		case EWVR_TouchId::Touchpad:
			_touched = UWaveVRController::RightController.GetTouchDown(WVR_InputId_Alias1_Touchpad);
			break;
		case EWVR_TouchId::Trigger:
			_touched = UWaveVRController::RightController.GetTouchDown(WVR_InputId_Alias1_Trigger);
			break;
		default:
			break;
		}
	}
#endif
	return _touched;
}

bool UWaveVRController::IsRightControllerButtonTouchedUp(EWVR_TouchId button_id)
{
	bool _touched = false;
#if WAVEVR_SUPPORTED_PLATFORMS
	if (FWaveVRHMD::IsWVRInitialized()) {
		switch (button_id)
		{
		case EWVR_TouchId::Touchpad:
			_touched = UWaveVRController::RightController.GetTouchUp(WVR_InputId_Alias1_Touchpad);
			break;
		case EWVR_TouchId::Trigger:
			_touched = UWaveVRController::RightController.GetTouchUp(WVR_InputId_Alias1_Trigger);
			break;
		default:
			break;
		}
	}
#endif
	return _touched;
}

bool UWaveVRController::IsLeftControllerButtonTouched(EWVR_TouchId button_id)
{
	bool _touched = false;
#if WAVEVR_SUPPORTED_PLATFORMS
	if (FWaveVRHMD::IsWVRInitialized()) {
		switch (button_id)
		{
		case EWVR_TouchId::Touchpad:
			_touched = UWaveVRController::LeftController.GetTouch(WVR_InputId_Alias1_Touchpad);
			break;
		case EWVR_TouchId::Trigger:
			_touched = UWaveVRController::LeftController.GetTouch(WVR_InputId_Alias1_Trigger);
			break;
		default:
			break;
		}
	}
#endif
	return _touched;
}

bool UWaveVRController::IsLeftControllerButtonTouchedDown(EWVR_TouchId button_id)
{
	bool _touched = false;
#if WAVEVR_SUPPORTED_PLATFORMS
	if (FWaveVRHMD::IsWVRInitialized()) {
		switch (button_id)
		{
		case EWVR_TouchId::Touchpad:
			_touched = UWaveVRController::LeftController.GetTouchDown(WVR_InputId_Alias1_Touchpad);
			break;
		case EWVR_TouchId::Trigger:
			_touched = UWaveVRController::LeftController.GetTouchDown(WVR_InputId_Alias1_Trigger);
			break;
		default:
			break;
		}
	}
#endif
	return _touched;
}

bool UWaveVRController::IsLeftControllerButtonTouchedUp(EWVR_TouchId button_id)
{
	bool _touched = false;
#if WAVEVR_SUPPORTED_PLATFORMS
	if (FWaveVRHMD::IsWVRInitialized()) {
		switch (button_id)
		{
		case EWVR_TouchId::Touchpad:
			_touched = UWaveVRController::LeftController.GetTouchUp(WVR_InputId_Alias1_Touchpad);
			break;
		case EWVR_TouchId::Trigger:
			_touched = UWaveVRController::LeftController.GetTouchUp(WVR_InputId_Alias1_Trigger);
			break;
		default:
			break;
		}
	}
#endif
	return _touched;
}

FVector2D UWaveVRController::GetRightControllerAxis(EWVR_TouchId button_id)
{
	FVector2D _vec = FVector2D::ZeroVector;
#if WAVEVR_SUPPORTED_PLATFORMS
	if (FWaveVRHMD::IsWVRInitialized())
	{
		switch (button_id)
		{
		case EWVR_TouchId::Touchpad:
			_vec = UWaveVRController::RightController.GetAxis(WVR_InputId_Alias1_Touchpad);
			break;
		case EWVR_TouchId::Trigger:
			_vec = UWaveVRController::RightController.GetAxis(WVR_InputId_Alias1_Trigger);
			break;
		default:
			break;
		}
	}
#endif
	return _vec;
}

FVector2D UWaveVRController::GetLeftControllerAxis(EWVR_TouchId button_id)
{
	FVector2D _vec = FVector2D::ZeroVector;
#if WAVEVR_SUPPORTED_PLATFORMS
	if (FWaveVRHMD::IsWVRInitialized())
	{
		switch (button_id)
		{
		case EWVR_TouchId::Touchpad:
			_vec = UWaveVRController::LeftController.GetAxis(WVR_InputId_Alias1_Touchpad);
			break;
		case EWVR_TouchId::Trigger:
			_vec = UWaveVRController::LeftController.GetAxis(WVR_InputId_Alias1_Trigger);
			break;
		default:
			break;
		}
	}
#endif
	return _vec;
}
#pragma endregion
