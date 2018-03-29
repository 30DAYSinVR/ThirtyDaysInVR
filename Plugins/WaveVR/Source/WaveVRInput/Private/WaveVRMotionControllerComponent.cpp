// Fill out your copyright notice in the Description page of Project Settings.

#include "WaveVRInput.h"
#include "WaveVRMotionControllerComponent.h"


// Sets default values for this component's properties
UWaveVRMotionControllerComponent::UWaveVRMotionControllerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// Whether the component is activated at creation or must be explicitly activated.
	bAutoActivate = true;

	if (FModuleManager::Get().IsModuleLoaded("WaveVRInput"))
	{
		// Initialization....
	}
}

void UWaveVRMotionControllerComponent::OnRegister()
{
	Super::OnRegister();

	MotionControllerComponent = NewObject<UMotionControllerComponent>(this, TEXT("MotionController"));
	MotionControllerComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MotionControllerComponent->SetupAttachment(this);
	MotionControllerComponent->RegisterComponent();

}


// Called when the game starts
void UWaveVRMotionControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get the Playercontroller to use for input events.
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

}


// Called every frame
void UWaveVRMotionControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	FRotator orientation = UWaveVRInputFunctionLibrary::GetWaveVRInputControllerOrientation(MotionControllerComponent->Hand);
#if WAVEVR_LOG_ANDROID_LOG
	LOGD(LogWaveVRMotionControllerComponent, "TickComponent() rotation: %s", TCHAR_TO_ANSI(*orientation.ToString()));
#else
	UE_LOG(LogWaveVRMotionControllerComponent, Display, TEXT("TickComponent() rotation: %s"), *orientation.ToString());
#endif
}

UMotionControllerComponent* UWaveVRMotionControllerComponent::GetMotionController() const
{
	return MotionControllerComponent;
}



