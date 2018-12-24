// Fill out your copyright notice in the Description page of Project Settings.

#include "LandscapeManager.h"
#include "DrawDebugHelpers.h"
#include "KismetProceduralMeshLibrary.h"
#include "EngineUtils.h"


// Sets default values
ALandscapeManager::ALandscapeManager()
{
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = mesh;
	// New in UE 4.17, multi-threaded PhysX cooking.
	mesh->bUseAsyncCooking = true;	
}

// Called when the game starts or when spawned
void ALandscapeManager::BeginPlay()
{
	Super::BeginPlay();
	//CreateTriangle();
	//DrawDebugSphere(GetWorld(), FVector(0, 0, 0), 50, 25, FColor(52, 220, 239), true);

	LoadMap();
	
}

// Called every frame
void ALandscapeManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// This is called when actor is spawned (at runtime or when you drop it into the world in editor)
void ALandscapeManager::PostActorCreated()
{
	Super::PostActorCreated();
	//CreateTriangle();
	//CreateShape();
}

// This is called when actor is already in level and map is opened
void ALandscapeManager::PostLoad()
{
	Super::PostLoad();
	//CreateTriangle();
	//CreateShape();
}

void ALandscapeManager::UpdateMesh()
{}
 
void ALandscapeManager::LogVertices(TArray<FVector> myArray) 
{
	for (int i = 0; i < myArray.Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("New Vector at position %i is %s"), i , *myArray[i].ToString());
	}
	
}

void ALandscapeManager::LogTriangles(TArray<int> myTriangles)
{
	int i = 0;

	while(i < myTriangles.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Triangle between: %i, %i, %i"), myTriangles[i], myTriangles[i+1], myTriangles[i+2]);
		i += 3;
	}
}

void ALandscapeManager::MyDrawDebugSphere(TArray<FVector> myArray) {

	for (int i = 0; i < myArray.Num(); i++) {
		DrawDebugPoint(GetWorld(), myArray[i], 10, FColor(52, 220, 239), true, 25000);
		//UE_LOG(LogTemp, Warning, TEXT("New Vector is %s"), *myArray[i].ToString());
	}
	
}

TArray<FVector> ALandscapeManager::BuildVertices(FVector position, TArray<int> dimensions) { // input should be just a chunk
	int cm = 100; //should be Global Variable with Worldsettings.WorldToMeter
	TArray<FVector> vertices;

	for (int i=0, y = position[1]; y < position[1] + dimensions[1]; y++) {
		for (int x = position[0]; x < position[0] + dimensions[0]; x++)
		{
			vertices.Add(FVector(x*cm, y*cm, 0*cm));
			i++;
		}
	}	LogVertices(vertices);
	return vertices;
}



TArray<int> ALandscapeManager::BuildTriangles(TArray<FVector> vertices, TArray<int> dimensions) {

	TArray<int> triangles;
	int vert = 0;
	int tris = 0;

	for (int y = 0; y < dimensions[1]; y++) {
		for (int x = 0; x < dimensions[0]; x++) {
			triangles.Add(vert);
			triangles.Add(vert + dimensions[0]);
			triangles.Add(vert + 1);
			triangles.Add(vert + 1);
			triangles.Add(vert + dimensions[0]);
			triangles.Add(vert + dimensions[0] + 1);
			vert++;
			tris += 6;
		}
	}
	//LogTriangles(triangles);
	return triangles;
}


void ALandscapeManager::BuildChunk(TArray<int> chunk)
{
	TArray<int> chunkSize = { 32,32,32 };

	FVector chunkStart = FVector(chunkSize[0] * (chunk[0]), chunkSize[1] * (chunk[1]), chunkSize[2] * (chunk[2]));

	UE_LOG(LogTemp, Warning, TEXT("ChunkStart is at %f, %f, %f"), chunkStart[0], chunkStart[1], chunkStart[2]);

	TArray<FVector> vertices;
	vertices = BuildVertices(chunkStart, chunkSize);

	

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

	mesh->CreateMeshSection_LinearColor(1, vertices, triangles, normals, UV0, vertexColors, tangents, true);


	// Enable collision data
	mesh->ContainsPhysicsTriMeshData(true);

	
}

void ALandscapeManager::LoadChunk(TArray<int> chunk)
{
}

TArray<int> ALandscapeManager::getActiveChunk(FVector coordinates)
{
	TArray<int> chunk;
	int chunkSize = 32;

	//UE_LOG(LogTemp, Warning, TEXT("Coordinates are %f, %f, %f"), coordinates[0], coordinates[1], coordinates[2]);
	
	for (int i = 0; i < 3; i++) {
		if (coordinates[i] == 0) chunk.Add(1);
		else if (coordinates[i] < 0) chunk.Add((int)floor(coordinates[i] / chunkSize));
		else if (coordinates[i] > 0) chunk.Add((int)ceil(coordinates[i]/chunkSize));
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("Active Chunk is %i, %i, %i"), chunk[0], chunk[1], chunk[2]);
	return chunk;
}

void ALandscapeManager::LoadMap()
{
	int radius = 2; // radius of chunks to load around the player
	
	//Get Player Position here
	FVector playerPosition = FVector(-1, 0, 0);

	TArray<int> activeChunk = getActiveChunk(playerPosition);
	//UE_LOG(LogTemp, Warning, TEXT("Active Chunk is %i, %i, %i"), activeChunk[0], activeChunk[1], activeChunk[2]);
	BuildChunk(activeChunk);

	/*
	for (int j = 0; j < 3; j++) { // iterate through x, y, z
		for (int i = 1; i <= radius; i++) { // iterate through radius
			TArray<int> chunkToLoad = activeChunk;
			chunkToLoad[j] = chunkToLoad[j]+i; 
			UE_LOG(LogTemp, Warning, TEXT("Chunk to load is %i, %i, %i"), chunkToLoad[0], chunkToLoad[1], chunkToLoad[2]);
			BuildChunk(chunkToLoad);
		}
	}
	*/
	
}
