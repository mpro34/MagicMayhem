// Fill out your copyright notice in the Description page of Project Settings.


#include "MagiPlayerState.h"
#include "MagiCharacter.h"
#include "MagicMayhem/PlayerController/MagiPlayerController.h"
#include "Net/UnrealNetwork.h"

void AMagiPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMagiPlayerState, Defeats);
	DOREPLIFETIME(AMagiPlayerState, Team);
}

void AMagiPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<AMagiCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AMagiPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore()); // Update score on client
		}
	}
}

void AMagiPlayerState::OnRep_Defeats()
{
	Character = Character == nullptr ? Cast<AMagiCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AMagiPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats); // Update defeats on server
		}
	}
}

void AMagiPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	Character = Character == nullptr ? Cast<AMagiCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AMagiPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore()); // Update score on server
		}
	}
}

void AMagiPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	Character = Character == nullptr ? Cast<AMagiCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AMagiPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats); // Update defeats on server
		}
	}
}

void AMagiPlayerState::OnRep_Team()
{
	AMagiCharacter* MChar = Cast<AMagiCharacter>(GetPawn());
	if (MChar)
	{
		MChar->SetTeamColor(Team);
	}
}

void AMagiPlayerState::SetTeam(ETeam TeamToSet)
{
	Team = TeamToSet;

	AMagiCharacter* MChar = Cast<AMagiCharacter>(GetPawn());
	if (MChar)
	{
		MChar->SetTeamColor(Team);
	}
}
