// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
//#include "WaveVRHMD.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BatteryStatusEvent.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBatteryStatusDelOneParam, float, percent);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBatteryStatusDelToBPOneParam, float, percent);

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBatteryStatusDelTwoParams, float, percent, int, device);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBatteryStatusDelToBPTwoParams, float, percent, int, device);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBatteryStatusDelNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBatteryStatusDelToBP);

UCLASS( ClassGroup=(WaveVR), meta=(BlueprintSpawnableComponent) )
class WAVEVR_API UBatteryStatusEvent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBatteryStatusEvent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void receiveStatusUpdateFromNative();

	static FBatteryStatusDelNative onBatteryStatusUpdateNative;

	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Event")
	FBatteryStatusDelToBP WaveVR_onBatteryStatusUpdate;
};

//#endif
