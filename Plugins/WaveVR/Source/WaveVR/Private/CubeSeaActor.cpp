// Fill out your copyright notice in the Description page of Project Settings.

#include "WaveVRPrivatePCH.h"
#include "CubeSeaActor.h"


// Sets default values
ACubeSeaActor::ACubeSeaActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

ACubeSeaActor::ACubeSeaActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CubeMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("CubeMesh"));
/*	static ConstructorHelpers::FObjectFinder<UBlueprint> CubeBP(TEXT("Blueprint'/WaveVR/Blueprints/CubeSeaNode.CubeSeaNode'"));
	if (CubeBP.Object) {
		CubeBlueprint = (UClass*)CubeBP.Object->GeneratedClass;
	} */
}

// Called when the game starts or when spawned
void ACubeSeaActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACubeSeaActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

