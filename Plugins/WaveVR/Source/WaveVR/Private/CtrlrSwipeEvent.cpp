// Fill out your copyright notice in the Description page of Project Settings.

#include "WaveVRPrivatePCH.h"
#include "CtrlrSwipeEvent.h"

FCtrlrSwipeLRDelNative UCtrlrSwipeEvent::onCtrlrSwipeLtoRUpdateNative;
FCtrlrSwipeRLDelNative UCtrlrSwipeEvent::onCtrlrSwipeRtoLUpdateNative;
FCtrlrSwipeUDDelNative UCtrlrSwipeEvent::onCtrlrSwipeUtoDUpdateNative;
FCtrlrSwipeDUDelNative UCtrlrSwipeEvent::onCtrlrSwipeDtoUUpdateNative;

// Sets default values for this component's properties
UCtrlrSwipeEvent::UCtrlrSwipeEvent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCtrlrSwipeEvent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	UCtrlrSwipeEvent::onCtrlrSwipeLtoRUpdateNative.AddDynamic(this, &UCtrlrSwipeEvent::receiveCtrlrSwipeLRUpdateFromNative);
	UCtrlrSwipeEvent::onCtrlrSwipeRtoLUpdateNative.AddDynamic(this, &UCtrlrSwipeEvent::receiveCtrlrSwipeRLUpdateFromNative);
	UCtrlrSwipeEvent::onCtrlrSwipeUtoDUpdateNative.AddDynamic(this, &UCtrlrSwipeEvent::receiveCtrlrSwipeUDUpdateFromNative);
	UCtrlrSwipeEvent::onCtrlrSwipeDtoUUpdateNative.AddDynamic(this, &UCtrlrSwipeEvent::receiveCtrlrSwipeDUUpdateFromNative);
}

void UCtrlrSwipeEvent::receiveCtrlrSwipeLRUpdateFromNative() {
	UE_LOG(LogTemp, Warning, TEXT("receiveCtrlrSwipeLRUpdateFromNative in UCtrlrSwipeEvent"));

	WaveVR_onCtrlrSwipeLRUpdate.Broadcast();
}

void UCtrlrSwipeEvent::receiveCtrlrSwipeRLUpdateFromNative() {
	UE_LOG(LogTemp, Warning, TEXT("receiveCtrlrSwipeRLUpdateFromNative in UCtrlrSwipeEvent"));

	WaveVR_onCtrlrSwipeRLUpdate.Broadcast();
}

void UCtrlrSwipeEvent::receiveCtrlrSwipeUDUpdateFromNative() {
	UE_LOG(LogTemp, Warning, TEXT("receiveCtrlrSwipeUDUpdateFromNative in UCtrlrSwipeEvent"));

	WaveVR_onCtrlrSwipeUDUpdate.Broadcast();
}

void UCtrlrSwipeEvent::receiveCtrlrSwipeDUUpdateFromNative() {
	UE_LOG(LogTemp, Warning, TEXT("receiveCtrlrSwipeDUUpdateFromNative in UCtrlrSwipeEvent"));

	WaveVR_onCtrlrSwipeDUUpdate.Broadcast();
}

// Called every frame
void UCtrlrSwipeEvent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

