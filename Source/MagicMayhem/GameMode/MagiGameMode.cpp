// Fill out your copyright notice in the Description page of Project Settings.


#include "MagiGameMode.h"
#include "MagiCharacter.h"
#include "MagicMayhem/PlayerController/MagiPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Gameframework/PlayerStart.h"
#include "MagicMayhem/PlayerState/MagiPlayerState.h"
#include "MagicMayhem/GameState/MagiGameState.h"

namespace MatchState
{
	const FName Cooldown = FName(TEXT("Cooldown"));
}

AMagiGameMode::AMagiGameMode()
{
	bDelayedStart = true;
}

void AMagiGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.0f)
		{
			StartMatch(); // Transition match state to InProgress
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.0f)
		{
			SetMatchState(MatchState::Cooldown); // Transition match state to Cooldown
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.0f)
		{
			RestartGame();
		}
	}
}

float AMagiGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	return BaseDamage;
}

void AMagiGameMode::PlayerEliminated(AMagiCharacter* VictimCharacter, AMagiPlayerController* VictimController, AMagiPlayerController* AttackingController)
{
	AMagiPlayerState* AttackingPlayerState = AttackingController != nullptr ? Cast<AMagiPlayerState>(AttackingController->PlayerState) : nullptr;
	AMagiPlayerState* VictimPlayerState = VictimController != nullptr ? Cast<AMagiPlayerState>(VictimController->PlayerState) : nullptr;
	AMagiGameState* MagiGameState = GetGameState<AMagiGameState>();
	
	if (AttackingPlayerState && AttackingPlayerState != VictimPlayerState && MagiGameState)
	{
		TArray<AMagiPlayerState*> PlayersCurrentlyInTheLead;
		for (auto LeadPlayer : MagiGameState->TopScoringPlayers)
		{
			PlayersCurrentlyInTheLead.Add(LeadPlayer);
		}

		AttackingPlayerState->AddToScore(1.0f); // On the server, update the score
		MagiGameState->UpdateTopScore(AttackingPlayerState);

		if (MagiGameState->TopScoringPlayers.Contains(AttackingPlayerState))
		{
			AMagiCharacter* Leader = Cast<AMagiCharacter>(AttackingPlayerState->GetPawn());
			if (Leader)
			{
				Leader->MulticastGainedTheLead();
			}
		}

		for (int32 i = 0; i < PlayersCurrentlyInTheLead.Num(); i++)
		{	// Player lost the lead
			if (!MagiGameState->TopScoringPlayers.Contains(PlayersCurrentlyInTheLead[i]))
			{
				AMagiCharacter* Loser = Cast<AMagiCharacter>(PlayersCurrentlyInTheLead[i]->GetPawn());
				if (Loser)
				{
					Loser->MulticastLostTheLead();
				}
			}
		}
	}

	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}

	if (VictimCharacter)
	{
		VictimCharacter->Eliminate(false);
	}

	// Iterate through all player controllers and broadcast the elimination announcement
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AMagiPlayerController* MagiPlayer = Cast<AMagiPlayerController>(*It);
		if (MagiPlayer && AttackingPlayerState && VictimPlayerState)
		{
			MagiPlayer->BroadcastElimination(AttackingPlayerState, VictimPlayerState);
		}
	}
}

void AMagiGameMode::RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController)
{
	if (EliminatedCharacter)
	{
		EliminatedCharacter->Reset();
		EliminatedCharacter->Destroy();
	}
	if (EliminatedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(EliminatedController, PlayerStarts[Selection]);
	}
}

void AMagiGameMode::PlayerLeftGame(AMagiPlayerState* PlayerLeaving)
{
	if (PlayerLeaving == nullptr) return;
	AMagiGameState* MagiGameState = GetGameState<AMagiGameState>();
	if (MagiGameState && MagiGameState->TopScoringPlayers.Contains(PlayerLeaving))
	{
		MagiGameState->TopScoringPlayers.Remove(PlayerLeaving);
	}
	AMagiCharacter* CharacterLeaving = Cast<AMagiCharacter>(PlayerLeaving->GetPawn());
	if (CharacterLeaving)
	{
		CharacterLeaving->Eliminate(true);
	}
}

void AMagiGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void AMagiGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AMagiPlayerController* MagiPlayer = Cast<AMagiPlayerController>(*It);
		if (MagiPlayer)
		{
			MagiPlayer->OnMatchStateSet(MatchState, bTeamsMatch);
		}
	}
}
