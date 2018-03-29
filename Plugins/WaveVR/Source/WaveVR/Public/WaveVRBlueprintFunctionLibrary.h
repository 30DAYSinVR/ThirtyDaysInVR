// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "WaveVRBlueprintFunctionLibrary.generated.h"

/** Defines the type of tracked devices*/
UENUM(BlueprintType)
enum class EWVR_DeviceType : uint8
{
    DeviceType_HMD,
    DeviceType_Controller_Right,
    DeviceType_Controller_Left
};

UENUM(BlueprintType)
enum class EWVR_Hand : uint8
{
    Hand_Controller_Right = 2,	// WVR_DeviceType::WVR_DeviceType_Controller_Right
    Hand_Controller_Left = 3	// WVR_DeviceType::WVR_DeviceType_Controller_Left
};

UENUM(BlueprintType)
enum class EWVR_DOF : uint8
{
    DOF_3,
    DOF_6,
    DOF_SYSTEM
};

UCLASS()
class WAVEVR_API UWaveVRBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "WaveVR|PoseManager")
    static bool GetDevicePose(FVector& OutPosition, FRotator& OutOrientation, EWVR_DeviceType type = EWVR_DeviceType::DeviceType_HMD);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|PoseManager")
	static FVector GetDeviceVelocity(EWVR_DeviceType type = EWVR_DeviceType::DeviceType_Controller_Right);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|PoseManager")
	static FVector GetDeviceAngularVelocity(EWVR_DeviceType type = EWVR_DeviceType::DeviceType_Controller_Right);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|PoseManager")
    static bool IsDeviceConnected(EWVR_DeviceType type);

    UFUNCTION(BlueprintCallable, Category = "WaveVR|PoseManager")
    static bool IsDevicePoseValid(EWVR_DeviceType type);

    UFUNCTION(BlueprintCallable, Category = "WaveVR|PoseManager")
    static int GetNumOfDoF(EWVR_DeviceType type);

    UFUNCTION(BlueprintCallable, Category = "WaveVR|PoseManager")
    static bool IsInputFocusCapturedBySystem();

    UFUNCTION(BlueprintCallable, Category = "WaveVR")
    static float getDeviceBatteryPercentage(EWVR_DeviceType type);

    UFUNCTION(BlueprintCallable, Category = "WaveVR")
    static FString GetRenderModelName(EWVR_Hand hand);

    UFUNCTION(BlueprintCallable, Category = "WaveVR")
    static bool GetRenderTargetSize(FIntPoint& OutSize);

    UFUNCTION(BlueprintCallable, Category = "WaveVR")
    static AActor * LoadCustomControllerModel(EWVR_DeviceType device, EWVR_DOF dof, FTransform transform);
};
