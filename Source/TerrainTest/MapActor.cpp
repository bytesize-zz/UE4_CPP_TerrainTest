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

	SpawnMap(5);
}

// Called every frame
void AMapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector myActiveChunk = getActiveChunk(getPlayerPositions());
	
	SpawnMap(5);
	//getChunksToLoad(myActiveChunk);
	getChunksToUnload(myActiveChunk);

}

void AMapActor::SpawnMap(int radius) {

	FVector myActiveChunk = getActiveChunk(getPlayerPositions());
	int x = myActiveChunk[0];
	int y = myActiveChunk[1];

	for (int i = -radius; i <=radius; i++) {
		for (int j = -radius; j <=radius; j++) {
			FVector newVector = FVector(x + i, y + j, 0);
			//check if the computed chunk is already loaded, if not add it chunksToLoad
			if (!isLoaded(newVector)) {
				SpawnChunk(newVector);
				//UE_LOG(LogTemp, Warning, TEXT("New chunk to load: %s"), *newVector.ToString());
			}
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("LoadedChunks2 Length: %s"), loadedChunks2.Num());
}



void AMapActor::SpawnChunk(FVector chunk)
{
	FVector spawnCoord = FVector(chunk[0] * (chunkSize-1)* 100, chunk[1] * (chunkSize-1)* 100, 0);
	AChunkActor *newChunk = GetWorld()->SpawnActor<AChunkActor>(spawnCoord, FRotator(0, 0, 0));

	newChunk->chunkPosition = chunk;

	if (newChunk) {
		loadedChunks.Add(newChunk);
	}
	//UE_LOG(LogTemp, Warning, TEXT("chunk: %s"), *chunk.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("Spawning new Chunk at position: %s"), *spawnCoord.ToString());
}

void AMapActor::DeleteChunk(AActor* chunk) {
	 
	if (chunk) {
		chunk->Destroy();
		
	}
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

	return activeChunk;
}

TArray<FVector> AMapActor::getChunksToLoad(FVector activeChunk)
{
	TArray<FVector> chunksToLoad;

	FVector myActiveChunk = getActiveChunk(getPlayerPositions());
	int x = myActiveChunk[0];
	int y = myActiveChunk[1];

	for (int x = 0; x < radius; x++) {
		for (int y = 0; y < radius; y++) {
			FVector newVector = FVector(activeChunk[0]+x, activeChunk[1]+y, 0);
			//loadedChunks.FindByKey(newVector);
			//check if the computed chunk is already loaded, if not add it chunksToLoad
			if (!isLoaded(newVector)) {
				SpawnChunk(newVector);
				//UE_LOG(LogTemp, Warning, TEXT("New chunk to load: %s"), *newVector.ToString());
			}			
		}
	}

	//logVectorArray(chunksToLoad);
	return chunksToLoad; 
}

//checks if we already have an Actor at a specific position
bool AMapActor::isLoaded(FVector chunk) {

	for (int i = 0; i < loadedChunks.Num(); i++) {
		if (loadedChunks[i]->chunkPosition == chunk) {
			return true;
		}
	}
	return false;
}

bool AMapActor::isInRadius(FVector chunk)
{
	FVector myActiveChunk = getActiveChunk(getPlayerPositions());

	// use abs() to compare chunk radius, without getting mixed up with negatives
	if ((abs(chunk[0] - myActiveChunk[0]) > radius) || (abs(chunk[1] - myActiveChunk[1]) > radius))
		return false;
	else
		return true;
}



TArray<FVector> AMapActor::getChunksToUnload(FVector activeChunk)
{
	TArray<FVector> chunksToUnload;

	for (int i = 0; i < loadedChunks.Num(); i++) {
		FVector chunkPosition = loadedChunks[i]->chunkPosition;

		if (!isInRadius(chunkPosition)) {
			//UE_LOG(LogTemp, Warning, TEXT("Chunk to delete: %s"), chunkPosition.ToString());
			DeleteChunk(loadedChunks[i]);
			loadedChunks.RemoveAt(i);
		}			
	}
}


void AMapActor::logVectorArray(TArray<FVector> myArray)
{
	for (int i = 0; i < myArray.Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("Vector: %s"),  *myArray[i].ToString());
	}
}


