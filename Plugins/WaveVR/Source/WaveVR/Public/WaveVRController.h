// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <chrono>
using namespace std::chrono;

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WaveVRController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(WaveVRController, Log, All);

/* Define the buttons of controller */
UENUM(BlueprintType)
enum class EWVR_InputId : uint8
{
	//WVR_InputId_Alias1_System = 0,
	Menu = 1,
	Grip = 2,
	DPad_Left = 3,
	DPad_Up = 4,
	DPad_Right = 5,
	DPad_Down = 6,
	Volume_Up = 7,
	Volume_Down = 8,
	Bumper = 9,
	Touchpad = 16,
	Trigger = 17,

	//Max = 32,
};

UENUM(BlueprintType)
enum class EWVR_TouchId : uint8
{
	Touchpad = 16,
	Trigger = 17,
};

class Device
{
public:
	static const int pressIdCount = 11;
	static const int touchIdCount = 2;

#if WAVEVR_SUPPORTED_PLATFORMS
	WVR_InputId pressIds[pressIdCount] = {
		WVR_InputId_Alias1_Menu,
		WVR_InputId_Alias1_Grip,
		WVR_InputId_Alias1_DPad_Left,
		WVR_InputId_Alias1_DPad_Up,
		WVR_InputId_Alias1_DPad_Right,  // 5
		WVR_InputId_Alias1_DPad_Down,
		WVR_InputId_Alias1_Volume_Up,
		WVR_InputId_Alias1_Volume_Down,
		WVR_InputId_Alias1_Bumper,
		WVR_InputId_Alias1_Touchpad,	// 10
		WVR_InputId_Alias1_Trigger
	};

	WVR_InputId touchIds[touchIdCount] = {
		WVR_InputId_Alias1_Touchpad,
		WVR_InputId_Alias1_Trigger
	};

	Device(WVR_DeviceType type);

	WVR_DeviceType DeviceType;

	bool AllowPressActionInAFrame(WVR_InputId id);
	bool GetPress(WVR_InputId _id);
	bool GetPressDown(WVR_InputId _id);
	bool GetPressUp(WVR_InputId _id);

	bool AllowTouchActionInAFrame(WVR_InputId id);
	bool GetTouch(WVR_InputId _id);
	bool GetTouchDown(WVR_InputId _id);
	bool GetTouchUp(WVR_InputId _id);

	FVector2D GetAxis(WVR_InputId _id);
#endif

private:
	bool preState_press[pressIdCount];
	bool curState_press[pressIdCount];
	milliseconds prevFrameCount_press[pressIdCount];

#if WAVEVR_SUPPORTED_PLATFORMS
	void Update_PressState(WVR_InputId _id);
#endif

	bool preState_touch[touchIdCount];
	bool curState_touch[touchIdCount];
	milliseconds prevFrameCount_touch[touchIdCount];

#if WAVEVR_SUPPORTED_PLATFORMS
	void Update_TouchState(WVR_InputId _id);
#endif

	FVector2D axis_touchpad;
	FVector2D axis_trigger;	// trigger has only x-axis
};

/**
 * 
 */
UCLASS(Blueprintable)
class WAVEVR_API UWaveVRController : public UObject
{
	GENERATED_BODY()

public:
	UWaveVRController();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsRightControllerButtonPressed(EWVR_InputId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsRightControllerButtonPressedDown(EWVR_InputId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsRightControllerButtonPressedUp(EWVR_InputId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsLeftControllerButtonPressed(EWVR_InputId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsLeftControllerButtonPressedDown(EWVR_InputId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsLeftControllerButtonPressedUp(EWVR_InputId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsRightControllerConnected();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsLeftControllerConnected();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsAnyButtonPressed();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsRightControllerButtonTouched(EWVR_TouchId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsRightControllerButtonTouchedDown(EWVR_TouchId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsRightControllerButtonTouchedUp(EWVR_TouchId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsLeftControllerButtonTouched(EWVR_TouchId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsLeftControllerButtonTouchedDown(EWVR_TouchId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsLeftControllerButtonTouchedUp(EWVR_TouchId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static FVector2D GetRightControllerAxis(EWVR_TouchId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static FVector2D GetLeftControllerAxis(EWVR_TouchId button_id);

private:
#if WAVEVR_SUPPORTED_PLATFORMS
	static Device RightController;
	static Device LeftController;
#endif
};
