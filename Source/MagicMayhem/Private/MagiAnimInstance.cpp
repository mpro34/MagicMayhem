// Fill out your copyright notice in the Description page of Project Settings.


#include "MagiAnimInstance.h"
#include "MagiCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MagicMayhem/Weapon/Weapon.h"
#include "MagicMayhem/MagicMayhemTypes/CombatState.h"

void UMagiAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MagiCharacter = Cast<AMagiCharacter>(TryGetPawnOwner());
}

void UMagiAnimInstance::NativeUpdateAnimation(float delta_time)
{
	Super::NativeUpdateAnimation(delta_time);

	if (MagiCharacter == nullptr)
	{
		MagiCharacter = Cast<AMagiCharacter>(TryGetPawnOwner());
	}

	if (MagiCharacter == nullptr)
	{
		return;
	}

	FVector velocity = MagiCharacter->GetVelocity();
	velocity.Z = 0.0f;
	Speed = velocity.Size();

	bIsInAir = MagiCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = MagiCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f ? true : false;
	bWeaponEquipped = MagiCharacter->IsWeaponEquipped();
	EquippedWeapon = MagiCharacter->GetEquippedWeapon();
	bIsCrouched = MagiCharacter->bIsCrouched;
	bAiming = MagiCharacter->IsAiming();
	TurningInPlace = MagiCharacter->GetTurningInPlace();
	bRotateRootBone = MagiCharacter->ShouldRotateRootBone();
	bEliminated = MagiCharacter->IsEliminated();
	bHoldingTheFlag = MagiCharacter->IsHoldingTheFlag();

	// Offset Yaw for strafing
	FRotator AimRotation = MagiCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(MagiCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, delta_time, 15.0f);
	YawOffset = DeltaRotation.Yaw;

	// Offset interpolation for Lean
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = MagiCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / delta_time;
	const float Interp = FMath::FInterpTo(Lean, Target, delta_time, 6.0f);
	Lean = FMath::Clamp(Interp, -90.0f, 90.0f);

	AO_Yaw = MagiCharacter->GetAOYaw();
	AO_Pitch = MagiCharacter->GetAOPitch();
	// Calculate transform for FABRIK which we can use the LeftHandSocket on each weapon to alter the left-hand transform.
	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && MagiCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		MagiCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		if (MagiCharacter->IsLocallyControlled() && MagiCharacter->GetHitTarget() != FVector::ZeroVector)
		{
			bLocallyControlled = true;
			FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
				RightHandTransform.GetLocation(),
				RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - MagiCharacter->GetHitTarget())
			);
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, delta_time, 30.0f);
		}
	}

	bUseFABRIK = MagiCharacter->GetCombatState() == ECombatState::ECS_Unoccupied;
	bool bFABRIKOverride = MagiCharacter->IsLocallyControlled() &&
		MagiCharacter->GetCombatState() == ECombatState::ECS_ThrowingGrenade &&
		MagiCharacter->bFinishedSwapping;
	if (bFABRIKOverride)
	{
		bUseFABRIK = !MagiCharacter->IsLocallyReloading();
	}
	bUseAimOffsets = MagiCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !MagiCharacter->GetDisableGameplay();
	bTransformRightHand = MagiCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !MagiCharacter->GetDisableGameplay();
}
