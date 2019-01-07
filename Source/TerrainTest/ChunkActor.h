// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
//#include "KismetProceduralMeshLibrary.h"
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
	virtual void Destroyed() override;

private:
	TArray<FVector> getVertices(int mode); // build our vertices for chunk at position with dimension mode 0 for terrain, 1 for water
	TArray<int> getTriangles(int mode); // mode 0 for terrain, 1 for water

	void getSlicedVertices(TArray<TArray<FVector>> &vertices, TArray<FVector2D> minMax);
	void getSlicedTriangles(TArray<TArray<int>> &triangles, TArray<TArray<FVector>> vertices);

	void setQuad(TArray<int> &triangles, int v00, int v10, int v01, int v11);
	TArray<FVector> CalculateNormals(TArray<FVector> vertices, TArray<int32> triangles);
	FVector SurfaceNormal(int indexA, int indexB, int indexC);
	TArray<FVector2D> getUVs(TArray<FVector> vertices);

	TArray<FVector> getVertices3D();
	void SetVertex(TArray<FVector>& vertices, int x, int y, int z);

	TArray<int> getTriangles3D(int vLength);
	void createTopFace(TArray<int> &triangles, int ring);
	void createBottomFace(TArray<int> &triangles, int ring, int vLength);
	
	void GenerateGroundMesh(); // chunk doesn't exist so build it from scratch
	void GenerateWaterMesh();

	//water physics
	void CreateWater();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void setChunkHeightMap(TArray<float> newHeightMap);
	void setChunkSize(int chunkSize);

	void setCmToMeter(int cmToMeter);
	void SetRenderQuality(int newRenderQuality);

private:
	UPROPERTY(VisibleAnywhere)
		USceneComponent * Root;
		UProceduralMeshComponent * completeMesh;
		UProceduralMeshComponent * outputMesh;
		UProceduralMeshComponent * tmpMesh;
		UProceduralMeshComponent * mesh;
		UProceduralMeshComponent * waterMesh;
		int RenderQuality = 1;
		TArray<FVector> terrainVertices;

public:		
	UPROPERTY(VisibleAnywhere)
		int cmToMeter; // distance between vertices or block size
		int tmp; // Temporary Name for chunkSize * renderQuality
	UPROPERTY(VisibleAnywhere)
		FVector chunkPosition;
	UPROPERTY(VisibleAnywhere)
		int chunkSize = 64;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
		UMaterialInterface* m_Ground;
		UMaterialInterface* m_Grass;
		UMaterialInterface* m_Mud;
		UMaterialInterface* m_Rock;

		UMaterialInterface* m_Water;
		APhysicsVolume * WaterPhysicActor;
		TArray<float> ChunkHeightMap;

 //either we set heightmap as public variable, or we need a function to get the height endpoints at each side, to pass to surrounding chunks

};


