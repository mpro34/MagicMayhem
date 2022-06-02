// Fill out your copyright notice in the Description page of Project Settings.

#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MagiCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "MagicMayhem/MagiComponents/LagCompensationComponent.h"
#include "MagicMayhem/PlayerController/MagiPlayerController.h"


void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());
	
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector Start = SocketTransform.GetLocation();

		// Maps hit character to the number of times hit
		TMap<AMagiCharacter*, uint32> HitMap;
		TMap<AMagiCharacter*, uint32> HeadShotMap;
		for (FVector_NetQuantize HitTarget : HitTargets)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			AMagiCharacter* MagiCharacter = Cast<AMagiCharacter>(FireHit.GetActor());
			if (MagiCharacter)
			{
				const bool bHeadShot = FireHit.BoneName.ToString() == FString("head");
				if (bHeadShot)
				{
					if (HeadShotMap.Contains(MagiCharacter)) HeadShotMap[MagiCharacter]++;
					else HeadShotMap.Emplace(MagiCharacter, 1);
				}
				else
				{
					if (HitMap.Contains(MagiCharacter)) HitMap[MagiCharacter]++;
					else HitMap.Emplace(MagiCharacter, 1);
				}

				// Particles for bullet hits
				if (ImpactParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(
						GetWorld(),
						ImpactParticles,
						FireHit.ImpactPoint,
						FireHit.ImpactNormal.Rotation()
					);
				}
				// Sound for bullet hits
				if (HitSound)
				{
					UGameplayStatics::PlaySoundAtLocation(
						this,
						HitSound,
						FireHit.ImpactPoint,
						0.5f,
						FMath::FRandRange(-0.5f, 0.5f)
					);
				}
			}
		}
		// Calculate body shots and head shots total damage
		TArray<AMagiCharacter*> HitCharacters;
		TMap<AMagiCharacter*, float> DamageMap;
		for (auto HitPair : HitMap)
		{
			if (HitPair.Key)
			{
				DamageMap.Emplace(HitPair.Key, HitPair.Value * Damage);
				HitCharacters.AddUnique(HitPair.Key);
			}
		}
		for (auto HeadShotPair : HeadShotMap)
		{
			if (HeadShotPair.Key)
			{
				if (DamageMap.Contains(HeadShotPair.Key)) DamageMap[HeadShotPair.Key] += HeadShotPair.Value * HeadShotDamage;
				else DamageMap.Emplace(HeadShotPair.Key, HeadShotPair.Value * HeadShotDamage);
				HitCharacters.AddUnique(HeadShotPair.Key);
			}
		}

		for (auto DamagePair : DamageMap)
		{
			if (DamagePair.Key && InstigatorController)
			{
				bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
				if (HasAuthority() && bCauseAuthDamage)
				{
					// Only applied on server
					UGameplayStatics::ApplyDamage(
						DamagePair.Key,			// player hit
						DamagePair.Value,		// Damage calculated in two loops above
						InstigatorController,
						GetOwner(),
						UDamageType::StaticClass()
					);
				}
			}
		}

		if (!HasAuthority() && bUseServerSideRewind)
		{
			OwningCharacter = OwningCharacter == nullptr ? Cast<AMagiCharacter>(OwnerPawn) : OwningCharacter;
			OwningController = OwningController == nullptr ? Cast<AMagiPlayerController>(InstigatorController) : OwningController;
			if (OwningCharacter && OwningController && OwningCharacter->GetLagCompensation() && OwningCharacter->IsLocallyControlled())
			{
				OwningCharacter->GetLagCompensation()->ShotgunServerScoreRequest(
					HitCharacters, 
					Start, 
					HitTargets,
					OwningController->GetServerTime() - OwningController->SingleTripTime
				);
			}
		}
	}
}

void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket == nullptr) return;

	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector TraceStart = SocketTransform.GetLocation();

	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	for (uint32 i = 0; i < NumberOfShrapnel; i++)
	{
		const FVector RandVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.0f, SphereRadius);
		const FVector EndLocation = SphereCenter + RandVector;
		FVector ToEndLocation = EndLocation - TraceStart;
		ToEndLocation = FVector(TraceStart + ToEndLocation * TRACE_LENGTH / ToEndLocation.Size());

		HitTargets.Add(ToEndLocation);
	}
}
