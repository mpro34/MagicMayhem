// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MagiGameState.generated.h"

/**
 * 
 */
UCLASS()
class MAGICMAYHEM_API AMagiGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(class AMagiPlayerState* ScoringPlayer);

	UPROPERTY(Replicated)
	TArray<AMagiPlayerState*> TopScoringPlayers;

	/* Teams */
	void RedTeamScores();
	void BlueTeamScores();

	TArray<AMagiPlayerState*> RedTeam;
	TArray<AMagiPlayerState*> BlueTeam;

	UPROPERTY(ReplicatedUsing = OnRep_RedTeamScore)
	float RedTeamScore = 0.0f;
	UFUNCTION()
	void OnRep_RedTeamScore();

	UPROPERTY(ReplicatedUsing = OnRep_BlueTeamScore)
	float BlueTeamScore = 0.0f;
	UFUNCTION()
	void OnRep_BlueTeamScore();

private:
	float TopScore = 0.0f;
};
