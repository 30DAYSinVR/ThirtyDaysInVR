// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
//#include "WaveVR.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IpdUpdateEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FIpdUpdateDelNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FIpdUpdateDelToBP);

UCLASS( ClassGroup=(WaveVR), meta=(BlueprintSpawnableComponent) )
class WAVEVR_API UIpdUpdateEvent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UIpdUpdateEvent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION()
	void receiveStatusUpdateFromNative();

	static FIpdUpdateDelNative onIpdUpdateNative;

	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Event")
	FIpdUpdateDelToBP onIpdUpdate;
};
