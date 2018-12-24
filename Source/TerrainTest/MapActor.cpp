// Fill out your copyright notice in the Description page of Project Settings.

#include "MapActor.h"
#include "ChunkActor.h"

// Sets default values
AMapActor::AMapActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = mesh;
	// New in UE 4.17, multi-threaded PhysX cooking.
	mesh->bUseAsyncCooking = true;

	FVector chunkSize = FVector(64, 64, 64);
	TArray<TArray<int>> chunkList;
	int multiplier = 100;
}

// Called when the game starts or when spawned
void AMapActor::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnChunk(TArray<int> {0, 0, 0});

}

// Called every frame
void AMapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMapActor::SpawnChunk(TArray<int> chunk)
{

	GetWorld()->SpawnActor<AChunkActor>(FVector(0, 0, 0), FRotator(0, 0, 0));
	GetWorld()->SpawnActor<AChunkActor>(FVector(-6300, 0, 0), FRotator(0, 0, 0));
	GetWorld()->SpawnActor<AChunkActor>(FVector(0, -6300, 0), FRotator(0, 0, 0));
	GetWorld()->SpawnActor<AChunkActor>(FVector(-6300, -6300, 0), FRotator(0, 0, 0));
}

