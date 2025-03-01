// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ChunkActor.h"
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

	TArray<float> CalculateHeightMapForChunk(FVector chunk);

	void BaseHeightMap(TArray<float> &heightMap, FVector chunk);
	void MountainHeightMap(TArray<float> &heightMap, FVector chunk);


	void SpawnMap(int radius);
	void SpawnChunk(FVector chunk);
	bool isLoaded(FVector chunk);
	bool isInRadius(FVector chunk); //checks if the given vector is within the loading radius

	void DeleteChunk(AActor* chunk);

	FVector getPlayerPositions();

	FVector getActiveChunk(FVector playerPosition); // calculates the chunk the player is within, from the given playerLocation
	void getChunksToUnload(); // get a list of all chunks, that are loaded but not within radius of one active player


private:
	UPROPERTY(VisibleAnywhere)
		int chunkSize = 64;

	UPROPERTY(VisibleAnywhere)
		TArray<AChunkActor*> loadedChunks; // list of Pointers to all loaded chunks
		TArray<FVector> playerPositions;
		USceneComponent * root;

	UPROPERTY(EditAnywhere)
		int cmToMeter; // distance between vertices or block size
		int chunkLoadRadius = 5;
		int renderQuality = 1; //ToDo
		
};