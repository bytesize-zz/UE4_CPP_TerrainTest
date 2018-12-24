// Fill out your copyright notice in the Description page of Project Settings.

#include "ChunkActor.h"
#include "PerlinNoise.h"

// Sets default values
AChunkActor::AChunkActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = mesh;
	// New in UE 4.17, multi-threaded PhysX cooking.
	mesh->bUseAsyncCooking = true;
}

// Called when the game starts or when spawned
void AChunkActor::BeginPlay()
{
	Super::BeginPlay();

	BuildChunk();
	
}

// Called every frame
void AChunkActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChunkActor::BuildChunk()
{
	int chunkSize = 64;
	
	TArray<FVector> vertices;
	vertices = BuildVertices(chunkSize);

	TArray<int32> triangles;
	triangles = BuildTriangles(vertices, chunkSize);

	TArray<FVector> normals;
	normals.Add(FVector(1, 0, 0));
	normals.Add(FVector(1, 0, 0));
	normals.Add(FVector(1, 0, 0));

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

	mesh->CreateMeshSection_LinearColor(0, vertices, triangles, normals, UV0, vertexColors, tangents, true);


	// Enable collision data
	mesh->ContainsPhysicsTriMeshData(true);

}

void AChunkActor::LoadChunk()
{
}

TArray<FVector> AChunkActor::BuildVertices(int chunkSize)
{
	int cm = 100; //should be Global Variable with Worldsettings.WorldToMeter
	TArray<FVector> vertices;
	unsigned int seed = 123;
	PerlinNoise pn(seed);

	for (int i = 0, y = 0; y < chunkSize + chunkSize; y++) {
		for (int x = 0; x < chunkSize; x++)
		{
			/*
			double X = (double)x / ((double)chunkSize);
			double Y = (double)y / ((double)chunkSize);
			*/
			double n = 20 * pn.noise(x, y, 0.8);
			
			n = n - floor(n);

			vertices.Add(FVector(x*cm, y*cm, n * cm));
			i++;
		}
	}	//LogVertices(vertices);
	return vertices;
}

TArray<int> AChunkActor::BuildTriangles(TArray<FVector> vertices, int chunkSize)
{
	TArray<int> triangles;
	int vert = 0;
	int tris = 0;

	for (int y = 0; y < chunkSize; y++) {
		for (int x = 0; x < chunkSize; x++) {
			triangles.Add(vert);
			triangles.Add(vert + chunkSize);
			triangles.Add(vert + 1);
			triangles.Add(vert + 1);
			triangles.Add(vert + chunkSize);
			triangles.Add(vert + chunkSize + 1);
			vert++;
			tris += 6;
		}
	}
	//LogTriangles(triangles);
	return triangles;
}

void AChunkActor::LogVertices(TArray<FVector> myArray)
{
	for (int i = 0; i < myArray.Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("New Vector at position %i is %s"), i, *myArray[i].ToString());
	}
}

void AChunkActor::LogTriangles(TArray<int> myTriangles)
{
	int i = 0;

	while (i < myTriangles.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Triangle between: %i, %i, %i"), myTriangles[i], myTriangles[i + 1], myTriangles[i + 2]);
		i += 3;
	}
}

