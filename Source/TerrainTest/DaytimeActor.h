// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DaytimeActor.generated.h"

UCLASS()
class TERRAINTEST_API ADaytimeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADaytimeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void moveLight();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


		USceneComponent * Root;

		UPROPERTY(EditAnywhere)
			AActor *directionalLight = NULL;
};
