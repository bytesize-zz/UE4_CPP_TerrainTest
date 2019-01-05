// Fill out your copyright notice in the Description page of Project Settings.

#include "ChunkActor.h"
#include "PerlinNoise.h"
#include "Librarys/FastNoise.h"

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

	outputMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("OutputMesh"));
	tmpMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TmpMesh"));
	completeMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("CompleteMesh"));
	waterMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedWaterMesh"));

	outputMesh->SetupAttachment(Root);
	waterMesh->SetupAttachment(Root);

	completeMesh->SetHiddenInGame(true);
	tmpMesh->SetHiddenInGame(true);

	waterMesh->SetCollisionProfileName(TEXT("NoCollision"));

	// New in UE 4.17, multi-threaded PhysX cooking.
	//mesh->bUseAsyncCooking = true;
	outputMesh->bUseAsyncCooking = true;	
	waterMesh->bUseAsyncCooking = true;
	
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> grassMaterial(TEXT("Material'/Game/StarterContent/Materials/M_Ground_Grass'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> mudMaterial(TEXT("Material'/Game/StarterContent/Materials/M_Ground_Gravel'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> rockMaterial(TEXT("Material'/Game/StarterContent/Materials/M_Rock_Basalt'"));

	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> waterMaterial(TEXT("Material'/Game/StarterContent/Materials/M_Water_Ocean'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> waterMaterial(TEXT("Material'/Game/MyMaterials/M_Water'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> groundMaterial(TEXT("Material'/Game/MyMaterials/M_Terrain'"));

	if (groundMaterial.Succeeded())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Found groundMaterial."));
		m_Ground = (UMaterialInterface*)groundMaterial.Object;
		m_Grass = (UMaterialInterface*)grassMaterial.Object;
		m_Mud = (UMaterialInterface*)mudMaterial.Object;
		m_Rock = (UMaterialInterface*)rockMaterial.Object;

		outputMesh->SetMaterial(0, m_Ground);
		outputMesh->SetMaterial(1, m_Grass);
		outputMesh->SetMaterial(2, m_Rock);
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
	FVector position = GetActorLocation();

	GenerateGroundMesh();
	CreateWater();
		
}

void AChunkActor::Destroyed()
{	}

// Called every frame
void AChunkActor::Tick(float DeltaTime){
	Super::Tick(DeltaTime);	
}

void AChunkActor::setChunkHeightMap(TArray<float> newHeightMap){
	ChunkHeightMap = newHeightMap;
}

void AChunkActor::setChunkSize(int newChunkSize){
	chunkSize = newChunkSize;
	tmp = chunkSize * RenderQuality;
}

void AChunkActor::setCmToMeter(int newCmToMeter) {
	cmToMeter = newCmToMeter;
}

void AChunkActor::SetRenderQuality(int newRenderQuality){
	RenderQuality = newRenderQuality;
	tmp = chunkSize * RenderQuality;
}

void AChunkActor::GenerateGroundMesh()
{
	TArray<FVector> vertices;
	//vertices = getVertices(0);

	//TestArea
	TArray<TArray<FVector>> v2 = {TArray<FVector>(), TArray<FVector>(), TArray<FVector>() }; // The 3 Terrain Types: Underwater, Grass, Mountain
	TArray<FVector2D> minMax = { FVector2D(-1000, 0), FVector2D(0, 20), FVector2D(20, 2000) }; // minMax Z Values for each Terrain Type
	getSlicedVertices(v2, minMax);
	//UE_LOG(LogTemp, Warning, TEXT("Vertex Count| Total: %i UnderWater: %i, Grass: %i, Mountain: %i"), v2[0].Num()+ v2[1].Num()+ v2[2].Num(),v2[0].Num(), v2[1].Num(), v2[2].Num());

	UE_LOG(LogTemp, Warning, TEXT("v2[1] Length: %i"), v2[1].Num());
	for (int i = 0; i < v2[1].Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("v2[1][%i]%s"), i, *v2[1][i].ToString());
	}
	



	TArray<int32> triangles;
	//triangles = getTriangles(0);

	//TestArea
	TArray<TArray<int>> t2 = { TArray<int> (), TArray<int> (), TArray<int> ()}; // the triangles for each Terrain
	TArray<int> verticesLength = { v2[0].Num() , v2[1].Num() , v2[2].Num() };	// number of vertices for each terrain
	getSlicedTriangles(t2, v2);

	UE_LOG(LogTemp, Warning, TEXT("t2[1] Length: %i"), t2[1].Num());
	for (int i = 0; i < t2[1].Num()-2; i+=3) {
		UE_LOG(LogTemp, Warning, TEXT("Triangle %i between: %s, %s, %s"), *v2[1][t2[1][i]].ToString(), *v2[1][t2[1][i+1]].ToString(), *v2[1][t2[1][i+2]].ToString());
	}


	TArray<FVector> normals;
	normals = getNormals(vertices, triangles);

	TArray<FVector2D> UV0;
	UV0 = getUVs(vertices);


	TArray<FProcMeshTangent> tangents;
	//tangents.Add(FProcMeshTangent(0, 1, 0));
	//tangents.Add(FProcMeshTangent(0, 1, 0));
	//tangents.Add(FProcMeshTangent(0, 1, 0));

	

	TArray<FLinearColor> vertexColors; // ToDo: find out, why this is nessecary
	//mesh->CreateMeshSection_LinearColor(0, vertices, triangles, normals, UV0, vertexColors, tangents, true);

	completeMesh->CreateMeshSection_LinearColor(0, v2[1], t2[1], normals, UV0, vertexColors, tangents, true);

	completeMesh->SetHiddenInGame(true);
	UKismetProceduralMeshLibrary::SliceProceduralMesh(completeMesh, FVector(1, 1, 2000), FVector(0, 0, 1), true, tmpMesh, EProcMeshSliceCapOption::CreateNewSectionForCap, m_Grass);
	UKismetProceduralMeshLibrary::GetSectionFromProceduralMesh(tmpMesh, 0, vertices, triangles, normals, UV0, tangents);
	//outputMesh->CreateMeshSection_LinearColor(2, vertices, triangles, normals, UV0, vertexColors, tangents, true);

	UKismetProceduralMeshLibrary::SliceProceduralMesh(completeMesh, FVector(1, 1, 0), FVector(0, 0, 1), true, tmpMesh, EProcMeshSliceCapOption::CreateNewSectionForCap, m_Grass);
	UKismetProceduralMeshLibrary::GetSectionFromProceduralMesh(tmpMesh, 0, vertices, triangles, normals, UV0, tangents);
	outputMesh->CreateMeshSection_LinearColor(1, vertices, triangles, normals, UV0, vertexColors, tangents, true);

	UKismetProceduralMeshLibrary::GetSectionFromProceduralMesh(completeMesh, 0, vertices, triangles, normals, UV0, tangents);
	//outputMesh->CreateMeshSection_LinearColor(2, vertices, triangles, normals, UV0, vertexColors, tangents, true);

	completeMesh = tmpMesh;
	UKismetProceduralMeshLibrary::SliceProceduralMesh(completeMesh, FVector(0, 0, 2000), FVector(0, 0, 1), true, tmpMesh, EProcMeshSliceCapOption::UseLastSectionForCap, m_Grass);
	UKismetProceduralMeshLibrary::GetSectionFromProceduralMesh(completeMesh, 0, vertices, triangles, normals, UV0, tangents);
	//outputMesh->CreateMeshSection_LinearColor(0, vertices, triangles, normals, UV0, vertexColors, tangents, true);


	// Enable collision data
	completeMesh->ClearAllMeshSections();
	tmpMesh->ClearAllMeshSections();
	outputMesh->ContainsPhysicsTriMeshData(true);
}

void AChunkActor::GenerateWaterMesh() {
	//while generating the water mesh, could we use the ground hightmap, to ask if
	//each x, y is needed to add to the watermesh? only when z of hightmap is below water level
	//Could we reduce the vertex count to 4 ?

	TArray<FVector> vertices;
	//vertices = getVertices3D();
	vertices = getVertices(1);

	TArray<int32> triangles;
	//triangles = getTriangles3D(vertices.Num());
	triangles = getTriangles(1);

	TArray<FVector> normals;
	//normals = getNormals(vertices, triangles);

	TArray<FVector2D> UV0;
	UV0 = getUVs(vertices);

	//ToDo: What are these tangents?
	TArray<FProcMeshTangent> tangents;
	/*tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));
	tangents.Add(FProcMeshTangent(0, 1, 0));*/	
	
	TArray<FLinearColor> vertexColors; // ToDo: find out, why this is nessecary

	waterMesh->CreateMeshSection_LinearColor(0, vertices, triangles, normals, UV0, vertexColors, tangents, false);
	waterMesh->CastShadow = false;

	// Enable collision data
	//waterMesh->ContainsPhysicsTriMeshData(true);
}

TArray<FVector> AChunkActor::getVertices(int mode)
{
	TArray<FVector> vertices;

	for (int y = 0; y <= chunkSize * RenderQuality; y++) {
		for (int x = 0; x <= chunkSize * RenderQuality; x++)
		{	
			double z = 0; 
			if(mode == 0){
				if (ChunkHeightMap.Num() > 0 )  //ToDo
					z = ChunkHeightMap[y * (chunkSize * RenderQuality + 1) + x];
				//else UE_LOG(LogTemp, Warning, TEXT("Heightmap Length is wrong: %i"), ChunkHeightMap.Num());
			}
			vertices.Add(FVector(x * cmToMeter,  y * cmToMeter, z*cmToMeter));

			//UE_LOG(LogTemp, Warning, TEXT("HeightMap: x=%i, y=%i | z = %f "), x, y, z);
		}
	}

	return vertices;
}

TArray<int> AChunkActor::getTriangles(int mode)
{
	TArray<int> triangles;

	for (int vert=0, y = 0; y < tmp; y++, vert++) {
		for (int x = 0; x < tmp; x++, vert++) {
			setQuad(triangles, vert, vert + 1, vert + tmp + 1, vert + tmp + 2);
			if(mode==1) setQuad(triangles, vert, vert + tmp + 1, vert + 1, vert + tmp + 2); //the bottom plane is only needed for water
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("Number of Triangles: %i"), triangles.Num());
	//LogTriangles(triangles);
	return triangles;
}



void AChunkActor::getSlicedVertices(TArray<TArray<FVector>>& vertices, TArray<FVector2D> minMax)
{

	for (int y = 0; y <= chunkSize * RenderQuality; y++) {
		for (int x = 0; x <= chunkSize * RenderQuality; x++)
		{
			double z = 0;
			if (ChunkHeightMap.Num() > 0) {
				z = ChunkHeightMap[y * (chunkSize * RenderQuality + 1) + x];

				for (int i = 0; i < minMax.Num(); i++) {
					if (z > minMax[i][0] && z <= minMax[i][1]){
						vertices[i].Add(FVector(x * cmToMeter, y * cmToMeter, z*cmToMeter));
					}
				}
			}
		}
	}

}

void AChunkActor::getSlicedTriangles(TArray<TArray<int>>& triangles, TArray<TArray<FVector>> vertices)
{
	for (int i = 1; i < 2; i++) {
		for (int vert = 0, y = 0; y < vertices[i].Num()/chunkSize; y++, vert++) {
			for (int x = 0; x < vertices[i].Num() /chunkSize; x++, vert++) {
				setQuad(triangles[i], vert, vert + 1, vert + chunkSize + 1, vert + chunkSize + 2);				
			}
		}
	}	
}

void AChunkActor::setQuad(TArray<int>& triangles, int v00, int v10, int v01, int v11)
{
	triangles.Add(v00);
	triangles.Add(v01);
	triangles.Add(v10);
	triangles.Add(v10);
	triangles.Add(v01);
	triangles.Add(v11);
}

TArray<FVector> AChunkActor::getNormals(TArray<FVector> vertices, TArray<int32> triangles)
{
	TArray<FVector> normals;
	FVector ba, ca;
	FVector newNormal;

	int a, b, c;

	//UE_LOG(LogTemp, Warning, TEXT("Vertice length is: %i"), vertices.Num());
	//UE_LOG(LogTemp, Warning, TEXT("Triangle length is: %i"), triangles.Num());

	for (int i = 0; i < vertices.Num(); i+=3) {
		a = 0; // vertices[triangles[i]];
		b = 0; //vertices[triangles[i+1]];
		c = -1;  //vertices[triangles[i + 2]];

		//ba = a - b;
		//ca = a - c;

		newNormal = FVector(a, b, c); //newNormal.CrossProduct(ba, ca);
		newNormal.Normalize(1);

		//UE_LOG(LogTemp, Warning, TEXT("New Normal is: %s"), *newNormal.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("new normal is: %s"),  *newNormal.ToString());
		normals.Add(newNormal);
	}
	/*std::for_each(int32, triangles) {	}*/

	return normals;
}

TArray<FVector2D> AChunkActor::getUVs(TArray<FVector> vertices)
{
	TArray<FVector2D> uv;
	int maximum = chunkSize* cmToMeter;

	for(int i = 0; i < vertices.Num(); i++) {
		double u = (vertices[i][0] /maximum ) * 10;
		double v = (vertices[i][1] /maximum ) * 10;
		uv.Add(FVector2D(u, v));
		//UE_LOG(LogTemp, Warning, TEXT("New UV is %f, %f"), u,v);
	}

	return uv;
}

TArray<FVector> AChunkActor::getVertices3D()
{
	TArray<FVector> vertices;

	float waterPosition = 0;
	
	for (int z = 0; z <= chunkSize; z++) {
		for (int x = 0; x <= chunkSize; x++) {
			SetVertex(vertices, x, 0, z);
		}
		for (int y = 1; y <= chunkSize; y++) {
			SetVertex(vertices, chunkSize, y, z);
		}
		for (int x = chunkSize - 1; x >= 0; x--) {

			SetVertex(vertices, x, chunkSize, z);
		}
		for (int y = chunkSize - 1; y > 0; y--) {
			SetVertex(vertices, 0, y, z);
		}
	}
	
	for (int y = 1; y < chunkSize; y++) {
		for (int x = 1; x < chunkSize; x++) {
			SetVertex(vertices, x, y, chunkSize);
		}
	}
	for (int y = 1; y < chunkSize; y++) {
		for (int x = 1; x < chunkSize; x++) {
			SetVertex(vertices, x, y, 0);
		}
	}

	return vertices;
}

void AChunkActor::SetVertex(TArray<FVector>& vertices, int x, int y, int z) {

	FVector inner = FVector(x*cmToMeter, y*cmToMeter, (z-chunkSize-1)*cmToMeter);
	vertices.Add(inner);

}

TArray<int> AChunkActor::getTriangles3D(int vLength)
{
	TArray<int> triangles;

	int ring = chunkSize * 4; // a complete ring around the edge of the ground 
	int vert = 0;

	for (int z = 0; z < chunkSize; z++, vert++) {
		for (int quad = 0; quad < ring - 1; quad++, vert++) {
			setQuad(triangles, vert, vert + 1, vert + ring, vert + ring + 1);
		}
		setQuad(triangles, vert, vert - ring + 1, vert + ring, vert + 1);
	}

	createTopFace(triangles, ring);
	createBottomFace(triangles, ring, vLength);

	return triangles;
}

void AChunkActor::createTopFace(TArray<int>& triangles, int ring)
{
	int v = ring * chunkSize;

	//First row of top face.
	for (int x = 0; x < chunkSize - 1; x++, v++) {
		setQuad(triangles, v, v + 1, v + ring - 1, v + ring);
	}
	setQuad(triangles, v, v + 1, v + ring - 1, v + 2);
	
	int vMin = ring * (chunkSize + 1) - 1;
	int vMid = vMin + 1;
	int vMax = v + 2;

	//All but the last row.
	for (int z = 1; z < chunkSize - 1; z++, vMin--, vMid++, vMax++) {
		//First quad of next row.
		setQuad(triangles, vMin, vMid, vMin - 1, vMid + chunkSize - 1);
		//Inner quads of this row
		for (int x = 1; x < chunkSize - 1; x++, vMid++) {
			setQuad(triangles, vMid, vMid + 1, vMid + chunkSize - 1, vMid + chunkSize);
		}
		//Last quad of this row
		setQuad(triangles, vMid, vMax, vMid + chunkSize - 1, vMax + 1);
	}

	//Last row
	int vTop = vMin - 2;
	//Last row first quad
	setQuad(triangles, vMin, vMid, vTop + 1, vTop);
	//inner quads
	for (int x = 1; x < chunkSize - 1; x++, vTop--, vMid++) {
		setQuad(triangles, vMid, vMid + 1, vTop, vTop - 1);
	}
	//last quad
	setQuad(triangles, vMid, vTop - 2, vTop, vTop - 1);
}

void AChunkActor::createBottomFace(TArray<int>& triangles, int ring, int vLength)
{
	int v = 1;
	int vMid = vLength - (chunkSize - 1) * (chunkSize - 1);

	//First row of bottom face.
	setQuad(triangles, ring - 1, vMid, 0, 1);
	for (int x = 1; x < chunkSize - 1; x++, v++, vMid++) {
		setQuad(triangles, vMid, vMid+1, v, v+1);
	}
	setQuad(triangles, vMid, v+2, v, v+1);

	int vMin = ring -2;
	vMid -= chunkSize -2;
	int vMax = v + 2;

	//All but the last row.
	for (int z = 1; z < chunkSize - 1; z++, vMin--, vMid++, vMax++) {
		//First quad of next row.
		setQuad(triangles, vMin, vMid + chunkSize - 1, vMin + 1, vMid);
		//Inner quads of this row
		for (int x = 1; x < chunkSize - 1; x++, vMid++) {
			setQuad(triangles, vMid + chunkSize - 1, vMid + chunkSize, vMid, vMid + 1);
		}
		//Last quad of this row
		setQuad(triangles, vMid + chunkSize - 1, vMax + 1, vMid, vMax);
	}

	//Last row
	int vTop = vMin - 1;
	//Last row first quad
	setQuad(triangles, vTop + 1, vTop, vTop + 2, vMid);
	//inner quads
	for (int x = 1; x < chunkSize - 1; x++, vTop--, vMid++) {
		setQuad(triangles, vTop, vTop - 1, vMid, vMid + 1);
	}
	//last quad
	setQuad(triangles, vTop, vTop - 1, vMid, vTop - 2);

}

void AChunkActor::CreateWater()
{
	GenerateWaterMesh();
}




