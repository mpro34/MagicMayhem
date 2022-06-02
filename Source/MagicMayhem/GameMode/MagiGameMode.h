// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MagiGameMode.generated.h"

namespace MatchState
{
	extern MAGICMAYHEM_API const FName Cooldown; // Match duration has been reach, display the winner and begin the cooldown timer
}

/**
 * 
 */
UCLASS()
class MAGICMAYHEM_API AMagiGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AMagiGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class AMagiCharacter* VictimCharacter, class AMagiPlayerController* VictimController, AMagiPlayerController* AttackingController);
	virtual void RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController);
	void PlayerLeftGame(class AMagiPlayerState* PlayerLeaving);
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.0f;
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.0f;
	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.0f;
	
	float LevelStartingTime = 0.0f;

	bool bTeamsMatch = false;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	float CountdownTime = 0.0f;

public:
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
};
