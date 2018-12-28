// Fill out your copyright notice in the Description page of Project Settings.

#include "ChunkActor.h"
#include "PerlinNoise.h"
#include "KismetProceduralMeshLibrary.h"

#include <iostream>     // std::cout
#include <algorithm>    // std::for_each
#include <vector>       // std::vector

#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>

#include "ConstructorHelpers.h" // Needed for Material import

// Sets default values
AChunkActor::AChunkActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SetActorTickInterval(10);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	waterMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedWaterMesh"));

	mesh->AttachTo(Root);
	waterMesh->AttachTo(Root);

	// New in UE 4.17, multi-threaded PhysX cooking.
	mesh->bUseAsyncCooking = true;
	waterMesh->bUseAsyncCooking = true;
	
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> groundMaterial(TEXT("Material'/Game/StarterContent/Materials/M_Ground_Grass'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> waterMaterial(TEXT("Material'/Game/StarterContent/Materials/M_Water_Ocean'"));

	if (groundMaterial.Succeeded())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Found groundMaterial."));
		m_Ground = (UMaterialInterface*)groundMaterial.Object;
		mesh->SetMaterial(0, m_Ground);
	}

	if (waterMaterial.Succeeded())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Found waterMaterial."));
		m_Water = (UMaterialInterface*)waterMaterial.Object;
		waterMesh->SetMaterial(0, m_Water);
	}


}

// Called when the game starts or when spawned
void AChunkActor::BeginPlay()
{
	Super::BeginPlay();

	double(*heightMap)[64] = new double[64][64];
	FVector position = GetActorLocation();

	BuildHeightMap(heightMap, position);
	BuildChunk(heightMap);
	generateWaterMesh();

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
	UV0 = getUVs(vertices, chunkSize);


	TArray<FProcMeshTangent> tangents;
	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));


	TArray<FLinearColor> vertexColors; // ToDo: find out, why this is nessecary
	mesh->CreateMeshSection_LinearColor(0, vertices, triangles, normals, UV0, vertexColors, tangents, true);
	// Enable collision data
	mesh->ContainsPhysicsTriMeshData(true);
	
}

void AChunkActor::generateWaterMesh() {

	int chunkSize = 64;

	TArray<FVector> vertices;
	vertices = getVertices(chunkSize, NULL);

	TArray<int32> triangles;
	triangles = getTriangles(chunkSize);

	TArray<FVector> normals;
	normals = getNormals(vertices, triangles);

	TArray<FVector2D> UV0;
	UV0 = getUVs(vertices, chunkSize);


	TArray<FProcMeshTangent> tangents;
	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));


	TArray<FLinearColor> vertexColors; // ToDo: find out, why this is nessecary

	waterMesh->CreateMeshSection_LinearColor(0, vertices, triangles, normals, UV0, vertexColors, tangents, true);
	// Enable collision data
	//waterMesh->ContainsPhysicsTriMeshData(true);
}


void AChunkActor::BuildHeightMap(double(*heightMap)[64], FVector position)
{
	unsigned int seed = 898;
	PerlinNoise pn(seed);

	double offsetMultiplier = 0.02; //ToDo: find out why the UProperty isn't working
	int extremaMultiplier = 30;
	   
	//UE_LOG(LogTemp, Warning, TEXT("Chunk Location is %s"), *position.ToString());

	double yOff = position[1] / cmToMeter * offsetMultiplier; //ToDo: this /1000 needs to be connected to the +=0.1
	//UE_LOG(LogTemp, Warning, TEXT("yOff: %f"), yOff);
	for (int y = 0; y < 64; y++) {
		double xOff = position[0] / cmToMeter * offsetMultiplier;
		//UE_LOG(LogTemp, Warning, TEXT("xOff: %f"), xOff);
		for (int x = 0; x < 64; x++) {
			double m = pn.noise(xOff, yOff, 0.8);
			m = m * extremaMultiplier;

			//UE_LOG(LogTemp, Warning, TEXT("Position i is %f, %f "), position[0] / 100, position[1] / 100);

			heightMap[x][y] = m;
			//UE_LOG(LogTemp, Warning, TEXT("hightMap %f"), m);
			xOff += offsetMultiplier;		}
		yOff += offsetMultiplier;
	}
}


TArray<FVector> AChunkActor::getVertices(int chunkSize, double(*heightMap)[64] = NULL)
{
	int cm = 100; //should be Global Variable with Worldsettings.WorldToMeter
	TArray<FVector> vertices;
	
	for (int y = 0; y < chunkSize; y++) {
		for (int x = 0; x < chunkSize; x++)
		{	
			double z;

			if(heightMap == NULL) z = 18;  
			else z = heightMap[x][y];

			//UE_LOG(LogTemp, Warning, TEXT("hightMap %f"), heightMap[x][y]);
			vertices.Add(FVector(x*cmToMeter, y*cmToMeter, z * cmToMeter));
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

TArray<FVector2D> AChunkActor::getUVs(TArray<FVector> vertices, int chunkSize)
{
	TArray<FVector2D> uv;
	int maximum = (chunkSize - 1) * 100;

	for(int i = 0; i < vertices.Num(); i++) {
		double u = (vertices[i][0] /maximum ) * 10;
		double v = (vertices[i][1] /maximum ) * 10;
		uv.Add(FVector2D(u, v));
		//UE_LOG(LogTemp, Warning, TEXT("New UV is %f, %f"), u,v);
	}

	return uv;
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

