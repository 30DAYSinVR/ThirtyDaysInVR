// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WaveVRInputFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UWaveVRInputFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	
public:
	UFUNCTION(BlueprintPure, Category = "WaveVR|Input")
	static FRotator GetWaveVRInputControllerOrientation(EControllerHand hand = EControllerHand::Right);

	UFUNCTION(BlueprintPure, Category = "WaveVR|Input")
	static FVector GetWaveVRInputControllerPosition(EControllerHand hand = EControllerHand::Right);

	UFUNCTION(BlueprintPure, Category = "WaveVR|Input")
	static ETrackingStatus GetWaveVRInputControllerTrackingStatus(EControllerHand hand = EControllerHand::Right);

	UFUNCTION(BlueprintPure, Category = "WaveVR|Input")
	static FVector GetWaveVRInputControllerVelocity(EControllerHand hand = EControllerHand::Right);

	UFUNCTION(BlueprintPure, Category = "WaveVR|Input")
	static FVector GetWaveVRInputControllerAngularVelocity(EControllerHand hand = EControllerHand::Right);

};
