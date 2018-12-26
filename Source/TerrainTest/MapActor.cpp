// Fill out your copyright notice in the Description page of Project Settings.

#include "MapActor.h"
#include "ChunkActor.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


// Sets default values
AMapActor::AMapActor()
{


	FVector chunkSize = FVector(64, 64, 64);
	TArray<TArray<int>> chunkList;
	int multiplier = 100;
}

// Called when the game starts or when spawned
void AMapActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickInterval(1);

	//SpawnChunk(TArray<int> {0, 0, 0});
	SpawnMap(10);
}

// Called every frame
void AMapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	getActiveChunk(getPlayerPositions());
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


//Track every Player on the map. but we only wan't to know the chunk, within the player is
FVector AMapActor::getPlayerPositions()
{
	APlayerController * myPlayer = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FVector thisPosition;
	playerPositions.Empty(); //clear old positions

	//search for player characters and add their position vector to the array
	thisPosition = myPlayer->GetPawn()->GetActorLocation();		
	playerPositions.Add(thisPosition);

	//UE_LOG(LogTemp, Warning, TEXT("Player is located at position: %s"), *playerPositions[0].ToString());
	return thisPosition;
	}

FVector AMapActor::getActiveChunk(FVector playerPosition)
{
	FVector activeChunk;

	for (int i = 0; i < 2; i++)
	{
		if (playerPosition[i] == 0) activeChunk[i] = 1;
		else if (playerPosition[i] < 0) activeChunk[i] = (int)floor(playerPosition[i] / (chunkSize *cmToMeter));
		else if (playerPosition[i] > 0) activeChunk[i] = (int)ceil(playerPosition[i] / (chunkSize *cmToMeter));
	}


	//activeChunk = FVector((int)floor(playerPosition[0] / (chunkSize * cmToMeter)), (int)floor(playerPosition[1] / (chunkSize* cmToMeter)), 0);
	//UE_LOG(LogTemp, Warning, TEXT("Active Chunk is: %s"), *activeChunk.ToString());

	/*
	for (int i = 0; i < 3; i++) {
		if (coordinates[i] == 0) activeChunk.Add(1);
		else if (coordinates[i] < 0) activeChunk.Add((int)floor(coordinates[i] / chunkSize));
		else if (coordinates[i] > 0) activeChunk.Add((int)ceil(coordinates[i] / chunkSize));
	}

	//UE_LOG(LogTemp, Warning, TEXT("Active Chunk is %i, %i, %i"), chunk[0], chunk[1], chunk[2]);
	return chunk;
	*/

	//UE_LOG(LogTemp, Warning, TEXT("Active Chunk is %f, %f, %f"), activeChunk[0], activeChunk[1], activeChunk[2]);
	return activeChunk;
}





