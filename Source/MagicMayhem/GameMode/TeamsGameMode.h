// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MagiGameMode.h"
#include "TeamsGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MAGICMAYHEM_API ATeamsGameMode : public AMagiGameMode
{
	GENERATED_BODY()

public:
	ATeamsGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage) override;
	virtual void PlayerEliminated(class AMagiCharacter* VictimCharacter, class AMagiPlayerController* VictimController, AMagiPlayerController* AttackingController) override;
	
protected:
	virtual void HandleMatchHasStarted() override;
};
