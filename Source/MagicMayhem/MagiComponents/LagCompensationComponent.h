// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

// Provides necessary information about a hit box at point in time.
USTRUCT(BlueprintType)
struct FHitBoxInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;
	UPROPERTY()
	FRotator Rotation;
	UPROPERTY()
	FVector BoxExtent;
};

// Stores information for all a character's hit boxes at a point in time.
USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time;

	UPROPERTY()
	TMap<FName, FHitBoxInformation> HitBoxInfo;

	UPROPERTY()
	AMagiCharacter* Character;
};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirmed;

	UPROPERTY()
	bool bHeadShot;
};

USTRUCT(BlueprintType)
struct FShotgunServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<AMagiCharacter*, uint32> HeadShots;

	UPROPERTY()
	TMap<AMagiCharacter*, uint32> BodyShots;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAGICMAYHEM_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULagCompensationComponent();
	friend class AMagiCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ShowFramePackage(const FFramePackage& Package, const FColor& Color);
	
	/* Hitscan SSR */
	FServerSideRewindResult ServerSideRewind(
		class AMagiCharacter* HitCharacter, 
		const FVector_NetQuantize& TraceStart, 
		const FVector_NetQuantize& HitLocation, 
		float HitTime
	);

	/* Projectile SSR */
	FServerSideRewindResult ProjectileServerSideRewind(
		class AMagiCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);

	/* Shotgun SSR */
	FShotgunServerSideRewindResult ShotgunServerSideRewind(
		const TArray<AMagiCharacter*>& HitCharacters,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations,
		float HitTime
	);

	/* RPCs for Score Request */
	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(
		AMagiCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,
		float HitTime
	);
	UFUNCTION(Server, Reliable)
	void ProjectileServerScoreRequest(
		AMagiCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);
	UFUNCTION(Server, Reliable)
	void ShotgunServerScoreRequest(
		const TArray<AMagiCharacter*>& HitCharacters,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations,
		float HitTime
	);

protected:
	virtual void BeginPlay() override;
	void SaveFramePackage(FFramePackage& Package);
	FFramePackage InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, const float HitTime);
	void CacheBoxPosiitons(AMagiCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveHitBoxes(AMagiCharacter* HitCharacter, const FFramePackage& Package);
	void ResetHitBoxes(AMagiCharacter* HitCharacter, const FFramePackage& Package);
	void EnabledCharacterMeshCollision(AMagiCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled);
	void SaveFramePackage();
	FFramePackage GetFrameToCheck(AMagiCharacter* HitCharacter, float HitTime);

	/* Hitscan */
	FServerSideRewindResult ConfirmHit(
		const FFramePackage& Package,
		AMagiCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation
	);

	/* Projectile */
	FServerSideRewindResult ProjectileConfirmHit(
		const FFramePackage& Package,
		AMagiCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);

	/* Shotgun */
	FShotgunServerSideRewindResult ShotgunConfirmHit(
		const TArray<FFramePackage>& FramePackages,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations
	);

private:
	UPROPERTY()
	AMagiCharacter* Character;

	UPROPERTY()
	class AMagiPlayerController* Controller;

	TDoubleLinkedList<FFramePackage> FrameHistory;

	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 4.0f;

public:	
		
};
