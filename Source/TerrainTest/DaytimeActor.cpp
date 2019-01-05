// Fill out your copyright notice in the Description page of Project Settings.

#include "DaytimeActor.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
ADaytimeActor::ADaytimeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;


}

// Called when the game starts or when spawned
void ADaytimeActor::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> childActors;
	this->GetAttachedActors(childActors);

	//ADirectionalLight
	for (int i = 0; i < childActors.Num(); i++) {
		if (childActors[i]->GetName() == "LightSource_0")
			directionalLight = childActors[i];
	}

	UE_LOG(LogTemp, Warning, TEXT("DaytimeActor child count: %s"), *directionalLight->GetName());
}

// Called every frame
void ADaytimeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	moveLight();
}

void ADaytimeActor::moveLight()
{
	if(directionalLight)
		directionalLight->AddActorLocalRotation(FRotator(0.01, 0, 0).Quaternion());
	else UE_LOG(LogTemp, Warning, TEXT("No Directional Light found."));
}
