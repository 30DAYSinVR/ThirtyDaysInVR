// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CtrlrSwipeEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCtrlrSwipeLRDelNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCtrlrSwipeRLDelNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCtrlrSwipeUDDelNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCtrlrSwipeDUDelNative);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCtrlrSwipeLRDelToBP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCtrlrSwipeRLDelToBP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCtrlrSwipeUDDelToBP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCtrlrSwipeDUDelToBP);

UCLASS(ClassGroup = (WaveVR), meta = (BlueprintSpawnableComponent))
class WAVEVR_API UCtrlrSwipeEvent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCtrlrSwipeEvent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		void receiveCtrlrSwipeLRUpdateFromNative();
	UFUNCTION()
		void receiveCtrlrSwipeRLUpdateFromNative();
	UFUNCTION()
		void receiveCtrlrSwipeUDUpdateFromNative();
	UFUNCTION()
		void receiveCtrlrSwipeDUUpdateFromNative();

	static FCtrlrSwipeLRDelNative onCtrlrSwipeLtoRUpdateNative;
	static FCtrlrSwipeRLDelNative onCtrlrSwipeRtoLUpdateNative;
	static FCtrlrSwipeUDDelNative onCtrlrSwipeUtoDUpdateNative;
	static FCtrlrSwipeDUDelNative onCtrlrSwipeDtoUUpdateNative;

	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Event")
		FCtrlrSwipeLRDelToBP WaveVR_onCtrlrSwipeLRUpdate;
	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Event")
		FCtrlrSwipeRLDelToBP WaveVR_onCtrlrSwipeRLUpdate;
	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Event")
		FCtrlrSwipeUDDelToBP WaveVR_onCtrlrSwipeUDUpdate;
	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Event")
		FCtrlrSwipeDUDelToBP WaveVR_onCtrlrSwipeDUUpdate;
};
