// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TerrainTestGameMode.h"
#include "TerrainTestCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

ATerrainTestGameMode::ATerrainTestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	//static ConstructorHelpers::FClassFinder<AActor> PlayerStart(TEXT("NetworkPlayerStart"));

	//TArray<AActor*> FoundActors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);

	//APawn* ResultPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, StartLocation, StartRotation, NULL, Instigator);
	//APlayerController::Possess(APawn* InPawn);

	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
