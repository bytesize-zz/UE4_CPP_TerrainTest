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
	void SpawnMap(int radius);
	void SpawnChunk(FVector chunk);

	FVector getPlayerPositions();

	void unloadChunks(); // delete one or all chunkActor beyond the load radius

	FVector getActiveChunk(FVector playerPosition); // calculates the chunk the player is within, from the given playerLocation
	TArray<FVector> getChunksToLoad(); // get a list of all chunks to load within the radius of each active player
	TArray<FVector> getChunksToUnload(); // get a list of all chunks, that are loaded but not within radius of each active player


private:
	UPROPERTY(VisibleAnywhere)
		int chunkSize = 64;

		TArray<FVector> loadedChunks; //list of chunks that are loaded Example: (0,0,0/1,0,0/-1,-1,0)

		TArray<FVector> playerPositions;
	UPROPERTY(VisibleAnywhere)
		int cmToMeter = 100; // distance between vertices or block size
		int radius = 0;
};


// TODO: keep track of players on the map, and build/load the chunks around them.