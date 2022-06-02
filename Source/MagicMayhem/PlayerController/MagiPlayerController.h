// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MagiPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);

/**
 * 
 */
UCLASS()
class MAGICMAYHEM_API AMagiPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield(float Shield, float MaxShield);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void SetHUDGrenades(int32 Grenades);
	void HideTeamScores();
	void InitTeamScores();
	void SetHUDRedTeamScore(int32 RedScore);
	void SetHUDBlueTeamScore(int32 BlueScore);

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	void CheckPing(float DeltaTime);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual float GetServerTime(); // Synced with server world clock
	virtual void ReceivedPlayer() override; // Sync with server clock as soon as possible
	void OnMatchStateSet(FName State, bool bTeamsMatch = false);
	void HandleMatchHasStarted(bool bTeamsMatch = false);
	void HandleCooldown();

	void BroadcastElimination(APlayerState* Attacker, APlayerState* Victim);
	float SingleTripTime = 0.0f;

	FHighPingDelegate HighPingDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	void SetHUDTime();
	void PollInit();

	/*
	Sync time between client and server
	*/
	// Requests the current server time, passing in the client's time when the request is sent.
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);
	// Reports the current server time to the client in response to ServerRequestServerTime.
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeofClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta = 0.0f; // difference between client and server time
	
	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.0f; // How often to sync client and server time, in seconds.

	float TimeSyncRunningTime = 0.0f;

	void CheckTimeSync(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidGame(FName StateOfMatch, float Warmup, float Match, float StartingTime, float Cooldown);

	void ShowReturnToMainMenu();

	UFUNCTION(Client, Reliable)
	void ClientElimAnnouncement(APlayerState* Attacker, APlayerState* Victim);

	void HighPingWarning();
	void StopHighPingWarning();

	UPROPERTY(ReplicatedUsing = OnRep_ShowTeamScores)
	bool bShowTeamScores = false;

	UFUNCTION()
	void OnRep_ShowTeamScores();

	FString GetInfoText(const TArray<class AMagiPlayerState*>& Players);
	FString GetTeamsInfoText(class AMagiGameState* MagiGameState);

private:
	UPROPERTY()
	class AMagiHUD* MagiHUD;
	UPROPERTY()
	class AMagiGameMode* MagiGameMode;

	/* Return to main menu */
	UPROPERTY(EditAnywhere, Category = HUD)
	TSubclassOf<class UUserWidget> ReturnToMainMenuWidget;
	UPROPERTY()
	class UReturnToMainMenu* ReturnToMainMenu;
	bool bReturnToMainMenuOpen = false;

	float LevelStartingTime = 0.0f;
	float MatchTime = 0.0f; // In Seconds
	float WarmupTime = 0.0f;
	float CooldownTime = 0.0f;
	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	bool bInitCharacterOverlay = false;
	// Cache below variables if the character overlay isn't valid yet
	float CachedHealth;
	float CachedMaxHealth;
	bool bInitializeHealth = false;
	float CachedShield;
	float CachedMaxShield;
	bool bInitializeShield = false;
	float CachedScore;
	bool bInitializeScore = false;
	int32 CachedDefeats;
	bool bInitializeDefeats = false;
	int32 CachedGrenades;
	bool bInitializeGrenades = false;
	float CachedCarriedAmmo;
	bool bInitializeCarriedAmmo = false;
	float CachedWeaponAmmo;
	bool bInitializeWeaponAmmo = false;

	float HighPingRunningTime = 0.0f;
	float PingAnimationRunningTime = 0.0f;

	UPROPERTY(EditAnywhere)
	float HighPingDuration = 5.0f;
	UPROPERTY(EditAnywhere)
	float CheckPingFrequency = 20.0f;

	UFUNCTION(Server, Reliable)
	void ServerReportPingStatus(bool bHighPing);
	
	UPROPERTY(EditAnywhere)
	float HighPingThreshold = 50.0f;
};
