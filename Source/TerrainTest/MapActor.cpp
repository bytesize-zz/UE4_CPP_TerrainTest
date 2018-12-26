// Fill out your copyright notice in the Description page of Project Settings.

#include "MapActor.h"
#include "ChunkActor.h"

// Sets default values
AMapActor::AMapActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickInterval(5);

	FVector chunkSize = FVector(64, 64, 64);
	TArray<TArray<int>> chunkList;
	int multiplier = 100;
}

// Called when the game starts or when spawned
void AMapActor::BeginPlay()
{
	Super::BeginPlay();
	
	//SpawnChunk(TArray<int> {0, 0, 0});
	SpawnMap(10);
}

// Called every frame
void AMapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//SpawnMap(5);
	//radius++;
}

void AMapActor::SpawnChunk(TArray<int> chunk)
{
	for(int i = 0; i<=20;i++){
		SpawnChunk(FVector(i, i, 0));
		SpawnChunk(FVector(-1*i, i, 0));
		SpawnChunk(FVector(i, -1*i, 0));
		SpawnChunk(FVector(-1*i, -1*i, 0));
	}
}

void AMapActor::SpawnMap(int radius) {

	for (int x = 0; x < radius; x++) {
		for (int y = 0; y < radius; y++) {
			SpawnChunk(FVector(x, y, 0));
			SpawnChunk(FVector(-x, y, 0));
			SpawnChunk(FVector(x, -y, 0));
			SpawnChunk(FVector(-x, -y, 0));

		}
	}
}

void AMapActor::SpawnChunk(FVector chunk)
{
	FVector spawnCoord = FVector((chunk[0] * (chunkSize-1)) * 100, (chunk[1] *( chunkSize-1)) * 100, chunk[2]);
	GetWorld()->SpawnActor<AChunkActor>(spawnCoord, FRotator(0, 0, 0));

	//UE_LOG(LogTemp, Warning, TEXT("Spawning new Chunk at position: %s"), *spawnCoord.ToString());
}



