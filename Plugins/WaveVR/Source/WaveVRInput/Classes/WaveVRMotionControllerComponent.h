// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MotionControllerComponent.h"
#include "WaveVRMotionControllerComponent.generated.h"


DEFINE_LOG_CATEGORY_STATIC(LogWaveVRMotionControllerComponent, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WAVEVRINPUT_API UWaveVRMotionControllerComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWaveVRMotionControllerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnRegister() override;

public:
	UFUNCTION(BlueprintCallable, Category = "WaveVR|MotionController")
	UMotionControllerComponent* GetMotionController() const;


private:

	UMotionControllerComponent * MotionControllerComponent;
	APlayerController* PlayerController;
};
