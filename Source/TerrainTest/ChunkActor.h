// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ChunkActor.generated.h"

UCLASS()
class TERRAINTEST_API AChunkActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunkActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void BuildChunk(); // chunk doesn't exist so build it from scratch
	void LoadChunk(); // chunk was already created, load it from the disk

	TArray<FVector> BuildVertices(int chunkSize); // build our vertices for chunk at position with dimension 
	TArray<int> BuildTriangles(TArray<FVector> vertices, int chunkSize);

	//Debug Logging
	void LogVertices(TArray<FVector> myArray);
	void LogTriangles(TArray<int> myTriangles);

private:
	UPROPERTY(VisibleAnywhere)
		UProceduralMeshComponent * mesh;
};


