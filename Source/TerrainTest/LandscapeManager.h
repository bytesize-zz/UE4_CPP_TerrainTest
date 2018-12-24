// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "LandscapeManager.generated.h"

UCLASS()
class TERRAINTEST_API ALandscapeManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALandscapeManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PostActorCreated();
	void PostLoad();

	void UpdateMesh();

	void BuildChunk(TArray<int> chunk); // chunk doesn't exist so build it from scratch
	void LoadChunk(TArray<int> chunk); // chunk was already created, load it from the disk
	TArray<int> getActiveChunk(FVector coordinates); //return the chunk, within which the coordinates are

	void LoadMap(); //Load or Build Chunks arround the Player

	void LogVertices(TArray<FVector> myArray);
	void LogTriangles(TArray<int> myTriangles);

	void MyDrawDebugSphere(TArray<FVector> myArray);

	TArray<FVector> BuildVertices(FVector position, TArray<int> dimensions); // build our vertices for chunk at position with dimension 

	TArray<int> BuildTriangles(TArray<FVector> vertices, TArray<int> dimensions);


	void CreateTriangle();

private:
	UPROPERTY(VisibleAnywhere)
		UProceduralMeshComponent * mesh;

};
