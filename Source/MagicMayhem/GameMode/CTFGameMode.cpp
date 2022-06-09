// Fill out your copyright notice in the Description page of Project Settings.


#include "CTFGameMode.h"
#include "MagicMayhem/Weapon/Flag.h"
#include "MagicMayhem/CaptureTheFlag/FlagZone.h"
#include "MagicMayhem/GameState/MagiGameState.h"

void ACTFGameMode::PlayerEliminated(class AMagiCharacter* VictimCharacter, class AMagiPlayerController* VictimController, AMagiPlayerController* AttackingController)
{
	AMagiGameMode::PlayerEliminated(VictimCharacter, VictimController, AttackingController);


}

void ACTFGameMode::FlagCaptured(class AFlag* Flag, class AFlagZone* Zone)
{
	bool bValidCapture = Flag->GetTeam() != Zone->Team;
	AMagiGameState* MagiGameState = Cast<AMagiGameState>(GameState);
	if (MagiGameState)
	{
		if (Zone->Team == ETeam::ET_BlueTeam)
		{
			MagiGameState->BlueTeamScores();
		}
		if (Zone->Team == ETeam::ET_RedTeam)
		{
			MagiGameState->RedTeamScores();
		}
	}
}