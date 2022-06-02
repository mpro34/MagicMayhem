// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamsGameMode.h"
#include "MagicMayhem/GameState/MagiGameState.h"
#include "MagicMayhem/PlayerState/MagiPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "MagicMayhem/PlayerController/MagiPlayerController.h"

ATeamsGameMode::ATeamsGameMode()
{
	bTeamsMatch = true;
}

// Used if a player joins mid-game
void ATeamsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AMagiGameState* MGameState = Cast<AMagiGameState>(UGameplayStatics::GetGameState(this));
	if (MGameState)
	{
		AMagiPlayerState* MagiPState = NewPlayer->GetPlayerState<AMagiPlayerState>();
		if (MagiPState && MagiPState->GetTeam() == ETeam::ET_NoTeam)
		{
			if (MGameState->BlueTeam.Num() >= MGameState->RedTeam.Num())
			{
				MGameState->RedTeam.AddUnique(MagiPState);
				MagiPState->SetTeam(ETeam::ET_RedTeam);
			}
			else
			{
				MGameState->BlueTeam.AddUnique(MagiPState);
				MagiPState->SetTeam(ETeam::ET_BlueTeam);
			}
		}
		
	}
}

void ATeamsGameMode::Logout(AController* Exiting)
{
	AMagiGameState* MGameState = Cast<AMagiGameState>(UGameplayStatics::GetGameState(this));
	AMagiPlayerState* MagiPState = Exiting->GetPlayerState<AMagiPlayerState>();
	if (MGameState && MagiPState)
	{
		if (MGameState->RedTeam.Contains(MagiPState))
		{
			MGameState->RedTeam.Remove(MagiPState);
		}
		if (MGameState->BlueTeam.Contains(MagiPState))
		{
			MGameState->BlueTeam.Remove(MagiPState);
		}
	}
}

float ATeamsGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	AMagiPlayerState* AttackerPState = Attacker->GetPlayerState<AMagiPlayerState>();
	AMagiPlayerState* VictimPState = Victim->GetPlayerState<AMagiPlayerState>();
	if (AttackerPState == nullptr || VictimPState == nullptr) return BaseDamage;
	if (VictimPState == AttackerPState)
	{
		return BaseDamage;
	}
	if (AttackerPState->GetTeam() == VictimPState->GetTeam())
	{
		return 0;
	}
	return BaseDamage;
}

void ATeamsGameMode::PlayerEliminated(class AMagiCharacter* VictimCharacter, class AMagiPlayerController* VictimController, AMagiPlayerController* AttackingController)
{
	Super::PlayerEliminated(VictimCharacter, VictimController, AttackingController);

	AMagiGameState* MGameState = Cast<AMagiGameState>(UGameplayStatics::GetGameState(this));
	AMagiPlayerState* AttackingPlayerState = AttackingController ? Cast<AMagiPlayerState>(AttackingController->PlayerState) : nullptr;
	if (MGameState && AttackingPlayerState)
	{
		if (AttackingPlayerState->GetTeam() == ETeam::ET_BlueTeam)
		{
			MGameState->BlueTeamScores();
		}
		if (AttackingPlayerState->GetTeam() == ETeam::ET_RedTeam)
		{
			MGameState->RedTeamScores();
		}
	}
}

void ATeamsGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	AMagiGameState* MGameState = Cast<AMagiGameState>(UGameplayStatics::GetGameState(this));
	if (MGameState)
	{
		for (auto PState : MGameState->PlayerArray)
		{
			AMagiPlayerState* MagiPState = Cast<AMagiPlayerState>(PState.Get());
			if (MagiPState && MagiPState->GetTeam() == ETeam::ET_NoTeam)
			{
				if (MGameState->BlueTeam.Num() >= MGameState->RedTeam.Num())
				{
					MGameState->RedTeam.AddUnique(MagiPState);
					MagiPState->SetTeam(ETeam::ET_RedTeam);
				}
				else
				{
					MGameState->BlueTeam.AddUnique(MagiPState);
					MagiPState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
		}
	}
}
