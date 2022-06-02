// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	if (NumberOfPlayers >= 4)
	{
		UWorld* world = GetWorld();
		if (world)
		{
			bUseSeamlessTravel = true;
			world->ServerTravel(FString("/Game/Maps/CityMap?listen"));
		}
	}
}

void ALobbyGameMode::StartCityMap()
{
	GLog->Log("Starting CityMap now...");
	UWorld* world = GetWorld();
	if (world)
	{
		bUseSeamlessTravel = true;
		world->ServerTravel(FString("/Game/Maps/CityMap?listen"));
	}
}
