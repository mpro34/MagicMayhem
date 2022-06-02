// Fill out your copyright notice in the Description page of Project Settings.


#include "MagiGameState.h"
#include "Net/UnrealNetwork.h"
#include "MagicMayhem/PlayerState/MagiPlayerState.h"
#include "MagicMayhem/PlayerController/MagiPlayerController.h"

void AMagiGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMagiGameState, TopScoringPlayers);
	DOREPLIFETIME(AMagiGameState, RedTeamScore);
	DOREPLIFETIME(AMagiGameState, BlueTeamScore);
}

void AMagiGameState::UpdateTopScore(AMagiPlayerState* ScoringPlayer)
{
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	else if (ScoringPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}

void AMagiGameState::RedTeamScores()
{
	++RedTeamScore;
	AMagiPlayerController* MagiPlayerController = Cast<AMagiPlayerController>(GetWorld()->GetFirstPlayerController());
	if (MagiPlayerController)
	{
		MagiPlayerController->SetHUDRedTeamScore(RedTeamScore);
	}
}

void AMagiGameState::BlueTeamScores()
{
	++BlueTeamScore;
	AMagiPlayerController* MagiPlayerController = Cast<AMagiPlayerController>(GetWorld()->GetFirstPlayerController());
	if (MagiPlayerController)
	{
		MagiPlayerController->SetHUDBlueTeamScore(BlueTeamScore);
	}
}

void AMagiGameState::OnRep_RedTeamScore()
{
	AMagiPlayerController* MagiPlayerController = Cast<AMagiPlayerController>(GetWorld()->GetFirstPlayerController());
	if (MagiPlayerController)
	{
		MagiPlayerController->SetHUDRedTeamScore(RedTeamScore);
	}
}

void AMagiGameState::OnRep_BlueTeamScore()
{
	AMagiPlayerController* MagiPlayerController = Cast<AMagiPlayerController>(GetWorld()->GetFirstPlayerController());
	if (MagiPlayerController)
	{
		MagiPlayerController->SetHUDBlueTeamScore(BlueTeamScore);
	}
}
