// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CubeSeaActor.generated.h"

UCLASS()
class ACubeSeaActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(Category = Meshes, VisibleAnywhere)
	UStaticMeshComponent* CubeMesh;

public:	
	// Sets default values for this actor's properties
	ACubeSeaActor();
	ACubeSeaActor(const FObjectInitializer& ObjectInitializer);
	TSubclassOf<class ACubeActor> CubeBlueprint;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
