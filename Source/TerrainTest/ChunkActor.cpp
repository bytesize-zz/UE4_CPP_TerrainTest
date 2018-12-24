// Fill out your copyright notice in the Description page of Project Settings.

#include "ChunkActor.h"
#include "PerlinNoise.h"
#include "KismetProceduralMeshLibrary.h"

#include <iostream>     // std::cout
#include <algorithm>    // std::for_each
#include <vector>       // std::vector

// Sets default values
AChunkActor::AChunkActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SetActorTickInterval(10);

	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = mesh;
	// New in UE 4.17, multi-threaded PhysX cooking.
	mesh->bUseAsyncCooking = true;

	
	
}

// Called when the game starts or when spawned
void AChunkActor::BeginPlay()
{
	Super::BeginPlay();

	double(*heightMap)[64] = new double[64][64];
	BuildHeightMap(heightMap);
	BuildChunk(heightMap);

}

// Called every frame
void AChunkActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
}

void AChunkActor::BuildChunk(double(*heightMap)[64])
{
	int chunkSize = 64;
	
	TArray<FVector> vertices;
	vertices = getVertices(chunkSize, heightMap);

	TArray<int32> triangles;
	triangles = getTriangles(chunkSize);

	TArray<FVector> normals;
	normals = getNormals(vertices, triangles);

	TArray<FVector2D> UV0;
	UV0.Add(FVector2D(0, 0));
	UV0.Add(FVector2D(10, 0));
	UV0.Add(FVector2D(0, 10));


	TArray<FProcMeshTangent> tangents;
	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));


	TArray<FLinearColor> vertexColors;
	vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));


	//LogTriVertices(triangles, vertices);

	mesh->CreateMeshSection_LinearColor(0, vertices, triangles, normals, UV0, vertexColors, tangents, true);
	

	// Enable collision data
	mesh->ContainsPhysicsTriMeshData(true);

}

void AChunkActor::BuildHeightMap(double(*heightMap)[64])
{
	unsigned int seed = 123;
	PerlinNoise pn(seed);	
	double yOff = 0;	
	for (int y = 0; y < 64; y++) {
		double xOff = 0;
		for (int x = 0; x < 64; x++) {
			double m = pn.noise(xOff, yOff, 0.1);
			m = m * 10;

			heightMap[x][y] = m;
			//UE_LOG(LogTemp, Warning, TEXT("hightMap %f"), m);
			xOff += 0.1;		}
		yOff += 0.1;
	}
}


TArray<FVector> AChunkActor::getVertices(int chunkSize, double(*heightMap)[64])
{
	int cm = 100; //should be Global Variable with Worldsettings.WorldToMeter
	TArray<FVector> vertices;
	
	for (int y = 0; y < chunkSize; y++) {
		for (int x = 0; x < chunkSize; x++)
		{	
			double z = heightMap[x][y];
			//UE_LOG(LogTemp, Warning, TEXT("hightMap %f"), heightMap[x][y]);
			vertices.Add(FVector(x*cm, y*cm, z * cm));
		}
	}	//LogVertices(vertices);
	return vertices;
}

TArray<int> AChunkActor::getTriangles(int chunkSize)
{
	TArray<int> triangles;

	int vert = 0;

	//TODO: Find out why we need to do chunksize-1
	for (int y = 0; y < chunkSize-1; y++) {
		for (int x = 0; x < chunkSize-1; x++) {
			triangles.Add(vert);
			triangles.Add(vert + chunkSize);
			triangles.Add(vert + 1);
			triangles.Add(vert + 1);
			triangles.Add(vert + chunkSize);
			triangles.Add(vert + chunkSize + 1);
			vert++;
		}
		vert++;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Triangle Length is: %i"), triangles.Num());
	//LogTriangles(triangles);
	return triangles;
}

TArray<FVector> AChunkActor::getNormals(TArray<FVector> vertices, TArray<int32> triangles)
{
	TArray<FVector> normals;
	FVector ba, ca;
	FVector newNormal;

	FVector a, b, c;

	for (int i = 0; i < triangles.Num(); i+=3) {
		a = vertices[triangles[i]];
		b = vertices[triangles[i+1]];
		c = vertices[triangles[i+2]];

		ba = a - b;
		ca = a - c;

		newNormal = newNormal.CrossProduct(ba, ca);
		newNormal.Normalize(1);

		//UE_LOG(LogTemp, Warning, TEXT("New Normal is: %s"), *newNormal.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("new normal is: %s"),  *newNormal.ToString());
		normals.Add(newNormal);
	}
	/*std::for_each(int32, triangles) {	}*/

	return normals;
}



void AChunkActor::LogVertices(TArray<FVector> myVertices)
{
	for (int i = 0; i < myVertices.Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("New Vector at position %i is %s"), i, *myVertices[i].ToString());
	}
}

void AChunkActor::LogTriangles(TArray<int> myTriangles)
{
	int i = 0;
	while (i < myTriangles.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Triangle between: %i, %i, %i"), myTriangles[i], myTriangles[i + 1], myTriangles[i + 2]);
		UE_LOG(LogTemp, Warning, TEXT("Triangle between: %i, %i, %i"), myTriangles[i +3], myTriangles[i + 4], myTriangles[i + 5]);
		i += 6;
	}
}

void AChunkActor::LogTriVertices(TArray<int> myTriangles, TArray<FVector> myVertices) {
	int i = 0;

	while (i < 400)
	{
		UE_LOG(LogTemp, Warning, TEXT("----------------------------"));
		UE_LOG(LogTemp, Warning, TEXT("Triangle between: %s, %s, %s"), *myVertices[myTriangles[i]].ToString(), *myVertices[myTriangles[i+1]].ToString(), *myVertices[myTriangles[i+2]].ToString());
		UE_LOG(LogTemp, Warning, TEXT("and Triangle between: %s, %s, %s"), *myVertices[myTriangles[i+3]].ToString(), *myVertices[myTriangles[i+4]].ToString(), *myVertices[myTriangles[i+5]].ToString());
		i += 6;
	}
}

