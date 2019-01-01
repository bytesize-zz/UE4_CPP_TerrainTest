// Fill out your copyright notice in the Description page of Project Settings.

#include "MapActor.h"
#include "ChunkActor.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Librarys/FastNoise.h"
#include "Matrix.h"

// Sets default values
AMapActor::AMapActor(){


	//FVector chunkSize = FVector(64, 64, 64);
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

	//SpawnMap(2);
	//SpawnChunk(FVector(0, 0, 0));
	//SpawnChunk(FVector(-1, 0, 0));
}

// Called every frame
void AMapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FVector myActiveChunk = getActiveChunk(getPlayerPositions());

	SpawnMap(chunkLoadRadius);
	getChunksToUnload();
}

TArray<float> AMapActor::GetHeightMapForChunk(FVector chunk)
{
	TArray<float> heightMap;

	FastNoise lowTerrainNoise;
	int lowTerrainAmplitude;
	FastNoise mountainNoise;
	int mountainAmplitude;

	lowTerrainNoise.SetFrequency(0.3);
	lowTerrainNoise.SetFractalOctaves(8);
	lowTerrainAmplitude = 1;

	mountainNoise.SetFrequency(0.03);
	mountainAmplitude = 7500;
	mountainNoise.SetFractalOctaves(11);

	int chunkXPos = 0;
	int chunkYPos = 0;

	for (int y = 0; y <= chunkSize; y++) {
		chunkYPos = (chunk[1] * chunkSize + y) * 100;
		for (int x = 0; x <= chunkSize; x++) {
			chunkXPos = (chunk[0] * chunkSize + x) * 100;

			heightMap.Add(lowTerrainNoise.GetNoise(chunkXPos, chunkYPos));			
		}
	}
	return heightMap;
}

void AMapActor::SpawnMap(int chunkLoadRadius) {

	FVector myActiveChunk = getActiveChunk(getPlayerPositions());
	int x = myActiveChunk[0];
	int y = myActiveChunk[1];

	for (int i = -chunkLoadRadius; i <= chunkLoadRadius; i++) {
		for (int j = -chunkLoadRadius; j <= chunkLoadRadius; j++) {
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
	FVector spawnCoord = FVector(chunk[0] * (chunkSize)* 100, chunk[1] * (chunkSize)* 100, 0);
	//UE_LOG(LogTemp, Warning, TEXT("Chunk with SpawnCoor: %s spawned at : %s"), *chunk.ToString(),  *spawnCoord.ToString());
	
	//AChunkActor *newChunk;
	const UWorld* World = GetWorld();
	const FTransform SpawnLocAndRotation = FTransform(FRotator(0,0,0), spawnCoord, FVector(1,1,1));
	AChunkActor * newChunk = GetWorld()->SpawnActorDeferred<AChunkActor>(AChunkActor::StaticClass(), SpawnLocAndRotation);
	newChunk->setChunkSize(chunkSize);
	newChunk->setChunkHeightMap(GetHeightMapForChunk(chunk));
	newChunk->chunkPosition = chunk;
	newChunk->FinishSpawning(SpawnLocAndRotation);


	//newChunk = GetWorld()->SpawnActor<AChunkActor>(spawnCoord, FRotator(0, 0, 0));



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
	if ((abs(chunk[0] - myActiveChunk[0]) > chunkLoadRadius) || (abs(chunk[1] - myActiveChunk[1]) > chunkLoadRadius))
		return false;
	else
		return true;
}



void AMapActor::getChunksToUnload()
{

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


