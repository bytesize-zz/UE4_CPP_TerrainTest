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

	void BuildChunk(double(*heightMap)[64]); // chunk doesn't exist so build it from scratch
	void LoadChunk(); // ToDo
	void SaveChunk(); // ToDo

	void BuildHeightMap(double(*heightMap)[64]);

	TArray<FVector> getVertices(int chunkSize, double(*heightMap)[64]); // build our vertices for chunk at position with dimension 
	TArray<int> getTriangles(int chunkSize);
	TArray<FVector> getNormals(TArray<FVector> vertices,  TArray<int32> triangles);

	//Debug Logging
	void LogVertices(TArray<FVector> myArray);
	void LogTriangles(TArray<int> myTriangles);

	void LogTriVertices(TArray<int> myTriangles, TArray<FVector> myVertices);

private:
	UPROPERTY(VisibleAnywhere)
		UProceduralMeshComponent * mesh;
		float multiplier;
public:		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
		UMaterialInterface* TheMaterial;

 //either we set heightmap as public variable, or we need a function to get the height endpoints at each side, to pass to surrounding chunks

};


