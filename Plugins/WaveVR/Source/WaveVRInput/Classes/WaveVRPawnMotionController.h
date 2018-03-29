// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "WaveVRPawnMotionController.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogWaveVRPawnMotionController, Log, All);

UCLASS()
class WAVEVRINPUT_API AWaveVRPawnMotionController : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AWaveVRPawnMotionController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	UPROPERTY(EditAnywhere)
	USceneComponent* OurVisibleComponent;

	// Input APIs
	void Move_XAxis(float AxisValue);
	void Move_YAxis(float AxisValue);
	void StartGrowing();
	void StopGrowing();

	// Input Parameters
	FVector CurrentPosition;
	bool bGrowing;
	//FRotator CurrentRotation;
};
