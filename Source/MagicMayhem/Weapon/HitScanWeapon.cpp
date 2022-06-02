 // Fill out your copyright notice in the Description page of Project Settings.

#include "HitScanWeapon.h"
#include "DrawDebugHelpers.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MagiCharacter.h"
#include "MagicMayhem/PlayerController/MagiPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "WeaponTypes.h"
#include "MagicMayhem/MagiComponents/LagCompensationComponent.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		FHitResult FireHit;
		WeaponTraceHit(Start, HitTarget, FireHit);

		AMagiCharacter* MagiCharacter = Cast<AMagiCharacter>(FireHit.GetActor());
		if (MagiCharacter && InstigatorController)
		{
			bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
			if (HasAuthority() && bCauseAuthDamage)
			{
				const float DamageToCause = FireHit.BoneName.ToString() == FString("head") ? HeadShotDamage : Damage;
				
				// Only applied on server
				UGameplayStatics::ApplyDamage(
					MagiCharacter,
					DamageToCause,
					InstigatorController,
					GetOwner(),
					UDamageType::StaticClass()
				);
			}
			if (!HasAuthority() && bUseServerSideRewind) // Only apply server side rewind on clients!
			{
				OwningCharacter = OwningCharacter == nullptr ? Cast<AMagiCharacter>(OwnerPawn) : OwningCharacter;
				OwningController = OwningController == nullptr ? Cast<AMagiPlayerController>(InstigatorController) : OwningController;
				if (OwningCharacter && OwningController && OwningCharacter->GetLagCompensation() && OwningCharacter->IsLocallyControlled())
				{
					OwningCharacter->GetLagCompensation()->ServerScoreRequest(
						MagiCharacter, 
						Start,
						HitTarget,
						OwningController->GetServerTime() - OwningController->SingleTripTime
					);
				}
			}
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
				FireHit.ImpactPoint
			);
		}
		// Muzzle Flash Scatter
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				MuzzleFlash,
				SocketTransform
			);
		}
		// Firing Sound
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				FireSound,
				GetActorLocation()
			);
		}
	}
}

void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;

		World->LineTraceSingleByChannel(
			OutHit,
			TraceStart,
			End,
			ECollisionChannel::ECC_Visibility
		);
		FVector BeamEnd = End;
		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
		}
		else
		{
			OutHit.ImpactPoint = End;
		}

		//DrawDebugSphere(GetWorld(), BeamEnd, 16.0f, 12.0f, FColor::Orange, true);

		if (BeamParticles)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				World,
				BeamParticles,
				TraceStart,
				FRotator::ZeroRotator,
				true
			);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
}
