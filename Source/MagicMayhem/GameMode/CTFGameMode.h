// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TeamsGameMode.h"
#include "CTFGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MAGICMAYHEM_API ACTFGameMode : public ATeamsGameMode
{
	GENERATED_BODY()
	
public:
	virtual void PlayerEliminated(class AMagiCharacter* VictimCharacter, class AMagiPlayerController* VictimController, AMagiPlayerController* AttackingController) override;
	void FlagCaptured(class AFlag* Flag, class AFlagZone* Zone);
};
