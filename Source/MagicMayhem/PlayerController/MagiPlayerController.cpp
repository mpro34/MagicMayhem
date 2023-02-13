// Fill out your copyright notice in the Description page of Project Settings.

#include "MagiPlayerController.h"
#include "MagicMayhem/HUD/MagiHUD.h"
#include "MagicMayhem/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MagicMayhem/Public/MagiCharacter.h"
#include "Net/UnrealNetwork.h"
#include "MagicMayhem/GameMode/MagiGameMode.h"
#include "MagicMayhem/HUD/Announcement.h"
#include "Kismet/GameplayStatics.h"
#include "MagicMayhem/MagiComponents/CombatComponent.h"
#include "MagicMayhem/GameState/MagiGameState.h"
#include "MagicMayhem/PlayerState/MagiPlayerState.h"
#include "MagicMayhem/HUD/ReturnToMainMenu.h"
#include "MagicMayhem/MagicMayhemTypes/Announcement.h"
#include "Components/Image.h"

void AMagiPlayerController::BeginPlay()
{
	Super::BeginPlay();

	MagiHUD = Cast<AMagiHUD>(GetHUD());
	UE_LOG(LogTemp, Warning, TEXT("Begin playercontroller play!"));
	ServerCheckMatchState();
}

void AMagiPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit(); // Wait to update the HUD until Character Overlay is valid.

	CheckPing(DeltaTime);
}

void AMagiPlayerController::CheckPing(float DeltaTime)
{
	HighPingRunningTime += DeltaTime;
	if (HighPingRunningTime > CheckPingFrequency)
	{
		PlayerState = PlayerState == nullptr ? GetPlayerState<APlayerState>() : PlayerState;
		if (PlayerState)
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerState->GetCompressedPing() * 4: %d"), PlayerState->GetCompressedPing() * 4);
			// ping is compressed; it's actually ping / 4
			if (PlayerState->GetCompressedPing() * 4 > HighPingThreshold)
			{
				HighPingWarning();
				PingAnimationRunningTime = 0.0f;
				ServerReportPingStatus(true);
			}
			else
			{
				ServerReportPingStatus(false);
			}
		}
		HighPingRunningTime = 0.0f;
	}
	// Only play the high ping animation for HighPingDuration seconds
	if (MagiHUD &&
		MagiHUD->CharacterOverlay &&
		MagiHUD->CharacterOverlay->HighPingAnimation &&
		MagiHUD->CharacterOverlay->IsAnimationPlaying(MagiHUD->CharacterOverlay->HighPingAnimation))
	{
		PingAnimationRunningTime += DeltaTime;
		if (PingAnimationRunningTime > HighPingDuration)
		{
			StopHighPingWarning();
		}
	}
}

// Is the ping too high?
void AMagiPlayerController::ServerReportPingStatus_Implementation(bool bHighPing)
{
	HighPingDelegate.Broadcast(bHighPing);
}


void AMagiPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMagiPlayerController, MatchState);
	DOREPLIFETIME(AMagiPlayerController, bShowTeamScores);
}

void AMagiPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent == nullptr) return;

	InputComponent->BindAction("Quit", IE_Pressed, this, &AMagiPlayerController::ShowReturnToMainMenu);
}

float AMagiPlayerController::GetServerTime()
{
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AMagiPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void AMagiPlayerController::SetHUDTime()
{
	float TimeLeft = 0.0f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::Cooldown) TimeLeft = WarmupTime + MatchTime + CooldownTime - GetServerTime() + LevelStartingTime;

	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	if (HasAuthority())
	{
		MagiGameMode = MagiGameMode == nullptr ? Cast<AMagiGameMode>(UGameplayStatics::GetGameMode(this)) : MagiGameMode;
		if (MagiGameMode)
		{
			SecondsLeft = FMath::CeilToInt(MagiGameMode->GetCountdownTime() + LevelStartingTime);
		}
	}
	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}
	CountdownInt = SecondsLeft;
}

void AMagiPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (MagiHUD && MagiHUD->CharacterOverlay)
		{
			CharacterOverlay = MagiHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				if (bInitializeHealth) SetHUDHealth(CachedHealth, CachedMaxHealth);
				if (bInitializeShield) SetHUDShield(CachedShield, CachedMaxShield);
				if (bInitializeScore) SetHUDScore(CachedScore);
				if (bInitializeDefeats) SetHUDDefeats(CachedDefeats);
				if (bInitializeCarriedAmmo) SetHUDCarriedAmmo(CachedCarriedAmmo);
				if (bInitializeWeaponAmmo) SetHUDWeaponAmmo(CachedWeaponAmmo);

				AMagiCharacter* MagiCharacter = Cast<AMagiCharacter>(GetPawn());
				if (MagiCharacter && MagiCharacter->GetCombat())
				{
					if (bInitializeGrenades) SetHUDGrenades(MagiCharacter->GetCombat()->GetGrenades());
				}
			}
		}
	}
}

void AMagiPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.0f;
	}
}

void AMagiPlayerController::ShowReturnToMainMenu()
{
	// Show the return to main menu widget
	if (ReturnToMainMenuWidget == nullptr) return;
	if (ReturnToMainMenu == nullptr)
	{
		ReturnToMainMenu = CreateWidget<UReturnToMainMenu>(this, ReturnToMainMenuWidget);
	}
	if (ReturnToMainMenu)
	{
		bReturnToMainMenuOpen = !bReturnToMainMenuOpen;
		if (bReturnToMainMenuOpen)
		{
			ReturnToMainMenu->MenuSetup();
		}
		else
		{
			ReturnToMainMenu->MenuTearDown();
		}
	}
}

void AMagiPlayerController::HighPingWarning()
{
	MagiHUD = MagiHUD == nullptr ? Cast<AMagiHUD>(GetHUD()) : MagiHUD;
	bool bHUDValid = MagiHUD && MagiHUD->CharacterOverlay && MagiHUD->CharacterOverlay->HighPingImage && MagiHUD->CharacterOverlay->HighPingAnimation;
	if (bHUDValid)
	{
		MagiHUD->CharacterOverlay->HighPingImage->SetOpacity(1.0f);
		MagiHUD->CharacterOverlay->PlayAnimation(
			MagiHUD->CharacterOverlay->HighPingAnimation,
			0.0f,
			5
		);
	}
}

void AMagiPlayerController::StopHighPingWarning()
{
	MagiHUD = MagiHUD == nullptr ? Cast<AMagiHUD>(GetHUD()) : MagiHUD;
	bool bHUDValid = MagiHUD && MagiHUD->CharacterOverlay && MagiHUD->CharacterOverlay->HighPingImage && MagiHUD->CharacterOverlay->HighPingAnimation;
	if (bHUDValid)
	{
		MagiHUD->CharacterOverlay->HighPingImage->SetOpacity(0.0f);
		if (MagiHUD->CharacterOverlay->IsAnimationPlaying(MagiHUD->CharacterOverlay->HighPingAnimation))
		{
			MagiHUD->CharacterOverlay->StopAnimation(MagiHUD->CharacterOverlay->HighPingAnimation);
		}
	}
}

void AMagiPlayerController::BroadcastElimination(APlayerState* Attacker, APlayerState* Victim)
{
	ClientElimAnnouncement(Attacker, Victim);
}

void AMagiPlayerController::ClientElimAnnouncement_Implementation(APlayerState* Attacker, APlayerState* Victim)
{
	APlayerState* Self = GetPlayerState<APlayerState>();
	if (Attacker && Victim && Self)
	{
		MagiHUD = MagiHUD == nullptr ? Cast<AMagiHUD>(GetHUD()) : MagiHUD;
		if (MagiHUD)
		{
			if (Attacker == Self && Victim != Self)
			{
				MagiHUD->AddElimAnnouncement("You", Victim->GetPlayerName());
				return;
			}
			if (Victim == Self && Attacker != Self)
			{
				MagiHUD->AddElimAnnouncement(Attacker->GetPlayerName(), "You");
				return;
			}
			if (Attacker == Victim && Attacker == Self)
			{
				MagiHUD->AddElimAnnouncement("You", "yourself");
				return;
			}
			if (Attacker == Victim && Attacker != Self)
			{
				MagiHUD->AddElimAnnouncement(Attacker->GetPlayerName(), "themselves");
				return;
			}
			MagiHUD->AddElimAnnouncement(Attacker->GetPlayerName(), Victim->GetPlayerName());
		}
	}
}

void AMagiPlayerController::ServerCheckMatchState_Implementation()
{
	AMagiGameMode* GameMode = Cast<AMagiGameMode>(UGameplayStatics::GetGameMode(this));
	UE_LOG(LogTemp, Warning, TEXT("ServerCheckMatchState_Implementation"));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		CooldownTime = GameMode->CooldownTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, LevelStartingTime, CooldownTime);
		UE_LOG(LogTemp, Warning, TEXT("Match State..."));
	}
}

void AMagiPlayerController::ClientJoinMidGame_Implementation(FName StateOfMatch, float Warmup, float Match, float StartingTime, float Cooldown)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	LevelStartingTime = StartingTime;
	CooldownTime = Cooldown;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);
	UE_LOG(LogTemp, Warning, TEXT("HERE"));
	if (MagiHUD && MatchState == MatchState::WaitingToStart)
	{
		MagiHUD->AddAnnouncement();
		UE_LOG(LogTemp, Warning, TEXT("Adding announcement"));
	}
}

void AMagiPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AMagiPlayerController::ClientReportServerTime_Implementation(float TimeofClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeofClientRequest;
	SingleTripTime = 0.5f * RoundTripTime;
	float CurrentServerTime = TimeServerReceivedClientRequest + SingleTripTime;
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

void AMagiPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	MagiHUD = MagiHUD == nullptr ? Cast<AMagiHUD>(GetHUD()) : MagiHUD;
	bool bHUDValid = MagiHUD && MagiHUD->CharacterOverlay && MagiHUD->CharacterOverlay->HealthBar && MagiHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		MagiHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		MagiHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		bInitializeHealth = true;
		CachedHealth = Health;
		CachedMaxHealth = Health;
	}
}

void AMagiPlayerController::SetHUDShield(float Shield, float MaxShield)
{
	MagiHUD = MagiHUD == nullptr ? Cast<AMagiHUD>(GetHUD()) : MagiHUD;
	bool bHUDValid = MagiHUD && MagiHUD->CharacterOverlay && MagiHUD->CharacterOverlay->ShieldBar && MagiHUD->CharacterOverlay->ShieldText;
	if (bHUDValid)
	{
		const float ShieldPercent = Shield / MaxShield;
		MagiHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);
		FString ShieldText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Shield), FMath::CeilToInt(MaxShield));
		MagiHUD->CharacterOverlay->ShieldText->SetText(FText::FromString(ShieldText));
	}
	else
	{
		bInitializeShield = true;
		CachedShield = Shield;
		CachedMaxShield = MaxShield;
	}
}

void AMagiPlayerController::SetHUDScore(float Score)
{
	MagiHUD = MagiHUD == nullptr ? Cast<AMagiHUD>(GetHUD()) : MagiHUD;
	bool bHUDValid = MagiHUD && MagiHUD->CharacterOverlay && MagiHUD->CharacterOverlay->ScoreAmount;
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::CeilToInt(Score));
		MagiHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	else
	{
		bInitializeScore = true;
		CachedScore = Score;
	}
}

void AMagiPlayerController::SetHUDDefeats(int32 Defeats)
{
	MagiHUD = MagiHUD == nullptr ? Cast<AMagiHUD>(GetHUD()) : MagiHUD;
	bool bHUDValid = MagiHUD && MagiHUD->CharacterOverlay && MagiHUD->CharacterOverlay->DefeatsAmount;
	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), (Defeats));
		MagiHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
	else
	{
		bInitializeDefeats = true;
		CachedDefeats = Defeats;
	}
}

void AMagiPlayerController::HideTeamScores()
{
	MagiHUD = MagiHUD == nullptr ? Cast<AMagiHUD>(GetHUD()) : MagiHUD;
	bool bHUDValid = MagiHUD && 
		MagiHUD->CharacterOverlay && 
		MagiHUD->CharacterOverlay->RedTeamScore && 
		MagiHUD->CharacterOverlay->BlueTeamScore &&
		MagiHUD->CharacterOverlay->ScoreSpacerText;
	if (bHUDValid)
	{
		MagiHUD->CharacterOverlay->RedTeamScore->SetText(FText());
		MagiHUD->CharacterOverlay->BlueTeamScore->SetText(FText());
		MagiHUD->CharacterOverlay->ScoreSpacerText->SetText(FText());
	}
}

void AMagiPlayerController::InitTeamScores()
{
	MagiHUD = MagiHUD == nullptr ? Cast<AMagiHUD>(GetHUD()) : MagiHUD;
	bool bHUDValid = MagiHUD &&
		MagiHUD->CharacterOverlay &&
		MagiHUD->CharacterOverlay->RedTeamScore &&
		MagiHUD->CharacterOverlay->BlueTeamScore &&
		MagiHUD->CharacterOverlay->ScoreSpacerText;
	if (bHUDValid)
	{
		FString Zero("0");
		FString Spacer("|");
		MagiHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString(Zero));
		MagiHUD->CharacterOverlay->BlueTeamScore->SetText(FText::FromString(Zero));
		MagiHUD->CharacterOverlay->ScoreSpacerText->SetText(FText::FromString(Spacer));
	}
}

void AMagiPlayerController::SetHUDRedTeamScore(int32 RedScore)
{
	MagiHUD = MagiHUD == nullptr ? Cast<AMagiHUD>(GetHUD()) : MagiHUD;
	bool bHUDValid = MagiHUD &&
		MagiHUD->CharacterOverlay &&
		MagiHUD->CharacterOverlay->RedTeamScore;
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), RedScore);
		MagiHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString(ScoreText));
	}
}

void AMagiPlayerController::SetHUDBlueTeamScore(int32 BlueScore)
{
	MagiHUD = MagiHUD == nullptr ? Cast<AMagiHUD>(GetHUD()) : MagiHUD;
	bool bHUDValid = MagiHUD &&
		MagiHUD->CharacterOverlay &&
		MagiHUD->CharacterOverlay->BlueTeamScore;
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), BlueScore);
		MagiHUD->CharacterOverlay->BlueTeamScore->SetText(FText::FromString(ScoreText));
	}
}

void AMagiPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	MagiHUD = MagiHUD == nullptr ? Cast<AMagiHUD>(GetHUD()) : MagiHUD;
	bool bHUDValid = MagiHUD && MagiHUD->CharacterOverlay && MagiHUD->CharacterOverlay->WeaponAmmoAmount;
	if (bHUDValid)
	{
		FString WeaponAmmoText = FString::Printf(TEXT("%d"), (Ammo));
		MagiHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(WeaponAmmoText));
	}
	else
	{
		bInitializeWeaponAmmo = true;
		CachedWeaponAmmo = Ammo;
	}
}

void AMagiPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	MagiHUD = MagiHUD == nullptr ? Cast<AMagiHUD>(GetHUD()) : MagiHUD;
	bool bHUDValid = MagiHUD && MagiHUD->CharacterOverlay && MagiHUD->CharacterOverlay->CarriedAmmoAmount;
	if (bHUDValid)
	{
		FString CarriedAmmoText = FString::Printf(TEXT("%d"), (Ammo));
		MagiHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(CarriedAmmoText));
	}
	else
	{
		bInitializeCarriedAmmo = true;
		CachedCarriedAmmo = Ammo;
	}
}

void AMagiPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	MagiHUD = MagiHUD == nullptr ? Cast<AMagiHUD>(GetHUD()) : MagiHUD;
	bool bHUDValid = MagiHUD && MagiHUD->CharacterOverlay && MagiHUD->CharacterOverlay->MatchCountdownText;
	if (bHUDValid)
	{
		if (CountdownTime < 0.0f)
		{
			MagiHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}

		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.0f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		MagiHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void AMagiPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	MagiHUD = MagiHUD == nullptr ? Cast<AMagiHUD>(GetHUD()) : MagiHUD;
	bool bHUDValid = MagiHUD && MagiHUD->Announcement && MagiHUD->Announcement->WarmupTime;
	if (bHUDValid)
	{
		if (CountdownTime < 0.0f)
		{
			MagiHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}

		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.0f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		MagiHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void AMagiPlayerController::SetHUDGrenades(int32 Grenades)
{
	MagiHUD = MagiHUD == nullptr ? Cast<AMagiHUD>(GetHUD()) : MagiHUD;
	bool bHUDValid = MagiHUD && MagiHUD->CharacterOverlay && MagiHUD->CharacterOverlay->GrenadesText;
	if (bHUDValid)
	{
		FString GrenadesAmt = FString::Printf(TEXT("%d"), (Grenades));
		MagiHUD->CharacterOverlay->GrenadesText->SetText(FText::FromString(GrenadesAmt));
	}
	else
	{
		bInitializeGrenades = true;
		CachedGrenades = Grenades;
	}
}

void AMagiPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AMagiCharacter* MagiCharacter = Cast<AMagiCharacter>(InPawn);
	if (MagiCharacter)
	{
		SetHUDHealth(MagiCharacter->GetHealth(), MagiCharacter->GetMaxHealth());
	}
}

void AMagiPlayerController::OnMatchStateSet(FName State, bool bTeamsMatch)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted(bTeamsMatch);
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AMagiPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AMagiPlayerController::HandleMatchHasStarted(bool bTeamsMatch)
{
	if (HasAuthority()) bShowTeamScores = bTeamsMatch;
	MagiHUD = MagiHUD == nullptr ? Cast<AMagiHUD>(GetHUD()) : MagiHUD;
	if (MagiHUD)
	{
		if (MagiHUD->CharacterOverlay == nullptr) MagiHUD->AddCharacterOverlay(); // Show overlay HUD only when match state goes to in-progress
		if (MagiHUD->Announcement)
		{
			MagiHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
		if (!HasAuthority()) return;
		if (bTeamsMatch)
		{
			InitTeamScores();
		}
		else
		{
			HideTeamScores();
		}
	}
}

void AMagiPlayerController::OnRep_ShowTeamScores()
{
	if (bShowTeamScores)
	{
		InitTeamScores();
	}
	else
	{
		HideTeamScores();
	}
}

FString AMagiPlayerController::GetInfoText(const TArray<class AMagiPlayerState*>& Players)
{
	AMagiPlayerState* MagiPlayerState = GetPlayerState<AMagiPlayerState>();
	if (MagiPlayerState == nullptr) return FString();

	FString InfoTextString;
	if (Players.Num() == 0)
	{
		InfoTextString = Announcement::ThereIsNoWinner;
	}
	else if (Players.Num() == 1 && Players[0] == MagiPlayerState)
	{
		InfoTextString = Announcement::YouAreTheWinner;
	}
	else if (Players.Num() == 1)
	{
		InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *Players[0]->GetPlayerName());
	}
	else if (Players.Num() > 1)
	{
		InfoTextString = Announcement::PlayersTiedForTheWin;
		InfoTextString.Append(FString("\n"));
		for (auto TiedPlayer : Players)
		{
			InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
		}
	}
	return InfoTextString;
}

FString AMagiPlayerController::GetTeamsInfoText(AMagiGameState* MagiGameState)
{
	if (MagiGameState == nullptr) return FString();
	FString InfoTextString;
	const int32 RedTeamScore = MagiGameState->RedTeamScore;
	const int32 BlueTeamScore = MagiGameState->BlueTeamScore;

	if (RedTeamScore == 0 && BlueTeamScore == 0)
	{
		InfoTextString = Announcement::ThereIsNoWinner;
	}
	else if (RedTeamScore == BlueTeamScore)
	{
		InfoTextString = FString::Printf(TEXT("%s\n"), *Announcement::TeamsTiedForTheWin);
		InfoTextString.Append(Announcement::RedTeam);
		InfoTextString.Append(TEXT("\n"));
		InfoTextString.Append(Announcement::BlueTeam);
		InfoTextString.Append(TEXT("\n"));
	}
	else if (RedTeamScore > BlueTeamScore)
	{
		InfoTextString = Announcement::RedTeamWins;
		InfoTextString.Append(TEXT("\n"));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::RedTeam, RedTeamScore));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::BlueTeam, BlueTeamScore));
	}
	else if (RedTeamScore < BlueTeamScore)
	{
		InfoTextString = Announcement::BlueTeamWins;
		InfoTextString.Append(TEXT("\n"));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::BlueTeam, BlueTeamScore));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::RedTeam, RedTeamScore));
	}

	return InfoTextString;
}

void AMagiPlayerController::HandleCooldown()
{
	MagiHUD = MagiHUD == nullptr ? Cast<AMagiHUD>(GetHUD()) : MagiHUD;
	if (MagiHUD)
	{
		MagiHUD->CharacterOverlay->RemoveFromParent();
		if (MagiHUD->Announcement && 
			MagiHUD->Announcement->AnnouncementText && 
			MagiHUD->Announcement->InfoText)
		{
			MagiHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText = Announcement::NewMatchStartsIn;
			MagiHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));
			
			AMagiGameState* MagiGameState = Cast<AMagiGameState>(UGameplayStatics::GetGameState(this));
			AMagiPlayerState* MagiPlayerState = GetPlayerState<AMagiPlayerState>();
			if (MagiGameState && MagiPlayerState)
			{
				TArray<AMagiPlayerState*> TopPlayers = MagiGameState->TopScoringPlayers;
				FString InfoTextString = bShowTeamScores ? GetTeamsInfoText(MagiGameState) : GetInfoText(TopPlayers);

				MagiHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}
		}
	}
	AMagiCharacter* MagiCharacter = Cast<AMagiCharacter>(GetPawn());
	if (MagiCharacter && MagiCharacter->GetCombat())
	{
		MagiCharacter->bDisableGameplay = true;
		MagiCharacter->GetCombat()->FireButtonPressed(false); // Force to off, to avoid bug of infinite press before entering cooldown
	}
}