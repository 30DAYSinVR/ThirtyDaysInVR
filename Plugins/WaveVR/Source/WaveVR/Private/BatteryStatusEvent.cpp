// Fill out your copyright notice in the Description page of Project Settings.

#include "WaveVRPrivatePCH.h"
#include "BatteryStatusEvent.h"

FBatteryStatusDelNative UBatteryStatusEvent::onBatteryStatusUpdateNative;

// Sets default values for this component's properties
UBatteryStatusEvent::UBatteryStatusEvent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBatteryStatusEvent::BeginPlay()
{
	Super::BeginPlay();

	UBatteryStatusEvent::onBatteryStatusUpdateNative.AddDynamic(this, &UBatteryStatusEvent::receiveStatusUpdateFromNative);
}

void UBatteryStatusEvent::receiveStatusUpdateFromNative() {
	UE_LOG(LogTemp, Warning, TEXT("receiveStatusUpdateFromNative in UBatteryStatusEvent"));

    WaveVR_onBatteryStatusUpdate.Broadcast();
}

// Called every frame
void UBatteryStatusEvent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

