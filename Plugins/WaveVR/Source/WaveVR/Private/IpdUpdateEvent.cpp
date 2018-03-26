// Fill out your copyright notice in the Description page of Project Settings.

#include "WaveVRPrivatePCH.h"
#include "IpdUpdateEvent.h"
#include "WaveVRHMD.h"

FIpdUpdateDelNative UIpdUpdateEvent::onIpdUpdateNative;

// Sets default values for this component's properties
UIpdUpdateEvent::UIpdUpdateEvent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UIpdUpdateEvent::BeginPlay()
{
	Super::BeginPlay();

	UIpdUpdateEvent::onIpdUpdateNative.AddDynamic(this, &UIpdUpdateEvent::receiveStatusUpdateFromNative);	
}

void UIpdUpdateEvent::receiveStatusUpdateFromNative() {
	UE_LOG(LogTemp, Warning, TEXT("UIpdUpdateEvent : receiveStatusUpdateFromNative in UIpdUpdateEvent"));

	onIpdUpdate.Broadcast();
}

// Called every frame
void UIpdUpdateEvent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

