// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "MapActor.generated.h"

UCLASS()
class TERRAINTEST_API AMapActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SpawnChunk(TArray<int> chunk);

	void SpawnMap(int radius);

	void SpawnChunk(FVector chunk);


private:
	UPROPERTY(VisibleAnywhere)
		int chunkSize = 64;
		TArray<TArray<int>> chunkList;
		int multiplier; // size of one "Block", distance between vertices 
		int radius = 0;
};
