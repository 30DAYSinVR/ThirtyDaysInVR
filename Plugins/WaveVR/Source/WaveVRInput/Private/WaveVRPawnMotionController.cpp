// Fill out your copyright notice in the Description page of Project Settings.

#include "WaveVRInput.h"
#include "WaveVRPawnMotionController.h"


// Sets default values
AWaveVRPawnMotionController::AWaveVRPawnMotionController()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bGrowing = false;

	// Set this pawn to be controlled by smallest number of player.
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Create an empty USceneComponent which is available attached component.
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Create camera.
	UCameraComponent* OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));
	// Attach camera to root component, set offset and rotation.
	OurCamera->AttachTo(RootComponent);
	OurCamera->SetRelativeLocation(FVector(-400.0f, 0.0f, 250.0f));
	OurCamera->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	OurCamera->bLockToHmd = false;

	// Create visible component and attach to root component.
	OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
	OurVisibleComponent->AttachTo(RootComponent);
}

// Called when the game starts or when spawned
void AWaveVRPawnMotionController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWaveVRPawnMotionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Process Growing
	{
		float CurrentScale = OurVisibleComponent->GetComponentScale().X;
		if (bGrowing)
		{
			//  double size in 1s
			CurrentScale += DeltaTime;
		}
		else
		{
			// half size.
			CurrentScale -= (DeltaTime * 0.5f);
		}

		CurrentScale = FMath::Clamp(CurrentScale, 1.0f, 2.0f);
		OurVisibleComponent->SetWorldScale3D(FVector(CurrentScale));
	}

	// Process Movement
	{
		if (!CurrentPosition.IsZero())
		{
			FVector NewLocation = GetActorLocation() + CurrentPosition;// (CurrentPosition * DeltaTime);
			OurVisibleComponent->SetRelativeLocation(NewLocation);
			//SetActorLocation(NewLocation);
		}
	}

	// Rotaton
	{
		//CurrentRotation = UWaveVRInputFunctionLibrary::GetWaveVRInputControllerOrientation(EControllerHand::Right);
	}
}

// Called to bind functionality to input
void AWaveVRPawnMotionController::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	LOG_NOARG(LogWaveVRPawnMotionController, "SetupPlayerInputComponent()");
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//PlayerInputComponent->BindAction("Grow", IE_Pressed, this, &AWaveVRPawnMotionController::StartGrowing);
	//PlayerInputComponent->BindAction("Grow", IE_Released, this, &AWaveVRPawnMotionController::StopGrowing);

	PlayerInputComponent->BindAction("TouchpadPress_Right", IE_Pressed, this, &AWaveVRPawnMotionController::StartGrowing);
	PlayerInputComponent->BindAction("TouchpadPress_Right", IE_Released, this, &AWaveVRPawnMotionController::StopGrowing);

	PlayerInputComponent->BindAction("Bumper_Right", IE_Pressed, this, &AWaveVRPawnMotionController::StartGrowing);
	PlayerInputComponent->BindAction("Bumper_Right", IE_Released, this, &AWaveVRPawnMotionController::StopGrowing);

	//PlayerInputComponent->BindAxis("MoveX", this, &AWaveVRPawnMotionController::Move_XAxis);
	//PlayerInputComponent->BindAxis("MoveY", this, &AWaveVRPawnMotionController::Move_YAxis);

	PlayerInputComponent->BindAxis("TouchpadAxisX_Right", this, &AWaveVRPawnMotionController::Move_XAxis);
	PlayerInputComponent->BindAxis("TouchpadAxisY_Right", this, &AWaveVRPawnMotionController::Move_YAxis);

}

void AWaveVRPawnMotionController::Move_XAxis(float AxisValue)
{
	LOGD(LogWaveVRPawnMotionController, "Move_XAxis() X=%f", AxisValue);
	CurrentPosition.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 100.0f;
}

void AWaveVRPawnMotionController::Move_YAxis(float AxisValue)
{
	LOGD(LogWaveVRPawnMotionController, "Move_XAxis() Y=%f", AxisValue);
	CurrentPosition.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 100.0f;
}

void AWaveVRPawnMotionController::StartGrowing()
{
	LOG_NOARG(LogWaveVRPawnMotionController, "StartGrowing()");
	bGrowing = true;
}

void AWaveVRPawnMotionController::StopGrowing()
{
	LOG_NOARG(LogWaveVRPawnMotionController, "StopGrowing()");
	bGrowing = false;
}

