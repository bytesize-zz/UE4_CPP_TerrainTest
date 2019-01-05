//Fill out your copyright notice in the Description page of Project Settings.

#include "MapActor.h"
#include "ChunkActor.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Librarys/FastNoise.h"
#include "Runtime/Core/Public/Math/UnrealMathUtility.h"


// Sets default values
AMapActor::AMapActor(){

	TArray<TArray<int>> chunkList;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	cmToMeter = 100 / RenderQuality;

}

// Called when the game starts or when spawned
void AMapActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickInterval(1);

	//SpawnMap(2);
	SpawnChunk(FVector(0, 0, 0));
	SpawnChunk(FVector(0, 1, 0));
	//SpawnChunk(FVector(0, 1, 0));
	//SpawnChunk(FVector(-1, 1, 0));
	//SpawnChunk(FVector(-1, -1, 0));
}

// Called every frame
void AMapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	cmToMeter = 100 / RenderQuality;

	//FVector myActiveChunk = getActiveChunk(getPlayerPositions());

	//SpawnMap(chunkLoadRadius);
	//getChunksToUnload();
}

TArray<float> AMapActor::GetHeightMapForChunk(FVector chunk)
{
	TArray<float> heightMap;
	FVector absoluteChunkPos = FVector(chunk[0] * chunkSize, chunk[1] * chunkSize, 0);

	BaseHeightMap(heightMap, absoluteChunkPos);
	MountainHeightMap(heightMap, absoluteChunkPos);

	return heightMap;
}

void AMapActor::BaseHeightMap(TArray<float> &heightMap, FVector pos)
{

	FastNoise baseTerrainNoise;
	int baseTerrainAmplitude;

	baseTerrainNoise.SetFrequency(0.003);
	baseTerrainNoise.SetNoiseType(FastNoise::Perlin);
	baseTerrainNoise.SetFractalOctaves(8);
	baseTerrainAmplitude = 10;

	int xStep = 0;
	int yStep = 0;

	float newHeight = 0;


	for (int y = 0; y <= chunkSize * RenderQuality; y++) {
		yStep = pos[1] +  y/RenderQuality;
		for (int x = 0; x <= chunkSize * RenderQuality; x++) {
			xStep = pos[0] +  x/RenderQuality;
			newHeight = baseTerrainNoise.GetNoise(xStep, yStep) * baseTerrainAmplitude;

			heightMap.Add(newHeight);
		}
	}
}

void AMapActor::MountainHeightMap(TArray<float> &heightMap, FVector pos)
{
	int mountainAmplitude;
	mountainAmplitude = 10;

	FastNoise mountainCube;
	mountainCube.SetFrequency(0.002);
	mountainCube.SetNoiseType(FastNoise::Cubic);
	mountainCube.SetFractalOctaves(7);

	FastNoise mountainCellular;
	mountainCellular.SetFrequency(0.005);
	mountainCellular.SetNoiseType(FastNoise::Cellular);
	mountainCellular.SetCellularReturnType(FastNoise::Distance2Add);
	mountainCellular.SetFractalOctaves(3);

	FastNoise mountainPerlin;
	mountainPerlin.SetFrequency(0.003);
	mountainPerlin.SetNoiseType(FastNoise::PerlinFractal);
	mountainPerlin.SetFractalOctaves(11);	
	   
	float newHeight = 0;

	int xStep = 0;
	int yStep = 0;

	for (int y = 0; y <= chunkSize * RenderQuality; y++) {
		yStep = pos[1] +  + y/RenderQuality;
		for (int x = 0; x <= chunkSize * RenderQuality; x++) {
			xStep = pos[0] + x/RenderQuality;
			float clamp = FMath::Clamp(mountainCube.GetNoise(xStep, yStep), 0.0f, 1.0f);

			float tmp = mountainCellular.GetNoise(xStep, yStep);
			newHeight = (tmp * tmp + abs(mountainPerlin.GetNoise(xStep, yStep))) * mountainAmplitude;
			
			heightMap[y * (chunkSize * RenderQuality + 1) + x] += clamp * newHeight * mountainAmplitude;
		}
	}

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
	FVector spawnCoord = FVector((chunk[0] * chunkSize)*cmToMeter*RenderQuality, (chunk[1] * chunkSize)*cmToMeter*RenderQuality, 0);

	//UE_LOG(LogTemp, Warning, TEXT("spawncoord: %s"), *spawnCoord.ToString());

	const UWorld* World = GetWorld();
	const FTransform SpawnLocAndRotation = FTransform(FRotator(0,0,0), spawnCoord, FVector(1,1,1));

	AChunkActor * newChunk = GetWorld()->SpawnActorDeferred<AChunkActor>(AChunkActor::StaticClass(), SpawnLocAndRotation);
	newChunk->AttachToComponent(Root, FAttachmentTransformRules::KeepWorldTransform);

	newChunk->setChunkSize(chunkSize);
	newChunk->setCmToMeter(cmToMeter);
	newChunk->SetRenderQuality(RenderQuality);
	newChunk->setChunkHeightMap(GetHeightMapForChunk(chunk));
	newChunk->chunkPosition = chunk;
	newChunk->FinishSpawning(SpawnLocAndRotation);

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
	if (myPlayer) {
		thisPosition = myPlayer->GetPawn()->GetActorLocation();
		playerPositions.Add(thisPosition);
	}


	//UE_LOG(LogTemp, Warning, TEXT("Player is located at position: %s"), *playerPositions[0].ToString());
	return thisPosition;
	}

FVector AMapActor::getActiveChunk(FVector playerPosition){
	FVector activeChunk;

	for (int i = 0; i < 2; i++)	{
		if (playerPosition[i] == 0) activeChunk[i] = 1;
		else if (playerPosition[i] < 0) activeChunk[i] = (int)floor(playerPosition[i] / (chunkSize * cmToMeter));
		else if (playerPosition[i] > 0) activeChunk[i] = (int)ceil(playerPosition[i] / (chunkSize * cmToMeter));
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


