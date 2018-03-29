// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <chrono>
using namespace std::chrono;

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WaveVRBlueprintFunctionLibrary.h"
#include "WaveVRController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWaveVRController, Log, All);

static const int InputButtonCount = 11;
UENUM(BlueprintType)
enum class EWVR_InputId : uint8
{
	//WVR_InputId_Alias1_System = 0,
	Menu = 1,		// WVR_InputId::WVR_InputId_Alias1_Menu
	Grip = 2,		// WVR_InputId::WVR_InputId_Alias1_Grip
	DPad_Left = 3,	// WVR_InputId::WVR_InputId_Alias1_DPad_Left
	DPad_Up = 4,	// WVR_InputId::WVR_InputId_Alias1_DPad_Up
	DPad_Right = 5,	// WVR_InputId::WVR_InputId_Alias1_DPad_Right
	DPad_Down = 6,	// WVR_InputId::WVR_InputId_Alias1_DPad_Down
	Volume_Up = 7,	// WVR_InputId::WVR_InputId_Alias1_Volume_Up
	Volume_Down = 8,// WVR_InputId::WVR_InputId_Alias1_Volume_Down
	Bumper = 9,		// WVR_InputId::WVR_InputId_Alias1_Bumper
	Touchpad = 16,	// WVR_InputId::WVR_InputId_Alias1_Touchpad
	Trigger = 17,	// WVR_InputId::WVR_InputId_Alias1_Trigger

	//Max = 32,
};

static const EWVR_InputId InputButton[InputButtonCount] =
{
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

static const int TouchButtonCount = 2;
UENUM(BlueprintType)
enum class EWVR_TouchId : uint8
{
	Touchpad = 16,	// WVR_InputId::WVR_InputId_Alias1_Touchpad
	Trigger = 17,	// WVR_InputId::WVR_InputId_Alias1_Trigger
};

static const EWVR_TouchId TouchButton[TouchButtonCount] =
{
	EWVR_TouchId::Touchpad,
	EWVR_TouchId::Trigger
};

class Device
{
public:
	Device(EWVR_Hand hand);

	EWVR_Hand Hand;

	bool AllowPressActionInAFrame(EWVR_InputId id);
	bool GetPress(EWVR_InputId id);
	bool GetPressDown(EWVR_InputId id);
	bool GetPressUp(EWVR_InputId id);

	bool AllowTouchActionInAFrame(EWVR_TouchId id);
	bool GetTouch(EWVR_TouchId id);
	bool GetTouchDown(EWVR_TouchId id);
	bool GetTouchUp(EWVR_TouchId id);

	bool AllowGetAxisInAFrame(EWVR_TouchId id);
	FVector2D GetAxis(EWVR_TouchId id);

private:
	bool preState_press[InputButtonCount];
	bool curState_press[InputButtonCount];
	milliseconds prevFrameCount_press[InputButtonCount];

	void Update_PressState(EWVR_InputId _id);

	bool preState_touch[TouchButtonCount];
	bool curState_touch[TouchButtonCount];
	milliseconds prevFrameCount_touch[TouchButtonCount];

	void Update_TouchState(EWVR_TouchId _id);

	FVector2D axis_touchpad;
	FVector2D axis_trigger;	// trigger has only x-axis
	milliseconds prevFrameCount_axis[TouchButtonCount];
};

UCLASS(Blueprintable)
class WAVEVR_API UWaveVRController : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	UWaveVRController();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsRightControllerButtonPressed(EWVR_InputId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsLeftControllerButtonPressed(EWVR_InputId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsRightControllerButtonTouched(EWVR_TouchId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsLeftControllerButtonTouched(EWVR_TouchId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsRightControllerConnected();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsLeftControllerConnected();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsAnyButtonPressed();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static FVector2D GetRightControllerAxis(EWVR_TouchId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static FVector2D GetLeftControllerAxis(EWVR_TouchId button_id);

	/*
	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsRightControllerButtonPressedDown(EWVR_InputId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsRightControllerButtonPressedUp(EWVR_InputId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsLeftControllerButtonPressedDown(EWVR_InputId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsLeftControllerButtonPressedUp(EWVR_InputId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsRightControllerButtonTouchedDown(EWVR_TouchId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsRightControllerButtonTouchedUp(EWVR_TouchId button_id);

	//UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsLeftControllerButtonTouchedDown(EWVR_TouchId button_id);

	//UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsLeftControllerButtonTouchedUp(EWVR_TouchId button_id);
	*/

private:
	static Device RightController;
	static Device LeftController;
};
