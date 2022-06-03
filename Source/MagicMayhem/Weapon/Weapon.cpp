// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "MagiCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "Casing.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MagicMayhem/PlayerController/MagiPlayerController.h"
#include "MagicMayhem/MagiComponents/CombatComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MagicMayhem/HUD/WeaponPickup.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Only set collision on the server!
	
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
}

void AWeapon::EnableCustomDepth(bool bEnable)
{
	if (WeaponMesh)
	{
		WeaponMesh->SetRenderCustomDepth(bEnable);
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (PickupWidget)
	{
		ShowPickupWidget(false);
		UWeaponPickup* WeaponPickupWidget = Cast<UWeaponPickup>(PickupWidget->GetWidget());
		if (WeaponPickupWidget && WeaponTexture)
		{
			WeaponPickupWidget->SetWeaponImage(WeaponTexture);
			WeaponPickupWidget->SetAmmoAmtText(Ammo);
		}
	}

	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME_CONDITION(AWeapon, bUseServerSideRewind, COND_OwnerOnly);
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		OwningCharacter = nullptr;
		OwningController = nullptr;
	}
	else
	{
		OwningCharacter = OwningCharacter == nullptr ? Cast<AMagiCharacter>(Owner) : OwningCharacter;
		if (OwningCharacter && OwningCharacter->GetEquippedWeapon() && OwningCharacter->GetEquippedWeapon() == this)
		{
			SetHUDAmmo();
		}
	}
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMagiCharacter* MagiCharacter = Cast<AMagiCharacter>(OtherActor);
	if (MagiCharacter)
	{
		MagiCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMagiCharacter* MagiCharacter = Cast<AMagiCharacter>(OtherActor);
	if (MagiCharacter)
	{
		MagiCharacter->SetOverlappingWeapon(nullptr);
	}
}

FRotator AWeapon::GetRandomRotator()
{
	const float pitch = FMath::FRandRange(-180.0f, 180.0f);
	const float yaw = FMath::FRandRange(-180.0f, 180.0f);
	const float roll = FMath::FRandRange(-180.0f, 180.0f);
	return FRotator(pitch, yaw, roll);
}

// Called on server
void AWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	SetHUDAmmo();
	if (HasAuthority())
	{
		ClientUpdateAmmo(Ammo);
	}
	else
	{
		++Sequence;
	}
}

void AWeapon::ClientUpdateAmmo_Implementation(int32 ServerAmmo)
{
	if (HasAuthority()) return;
	Ammo = ServerAmmo;
	--Sequence;
	Ammo -= Sequence;
	SetHUDAmmo();
}

void AWeapon::AddAmmo(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);
	SetHUDAmmo();
	ClientAddAmmo(AmmoToAdd);
}

void AWeapon::ClientAddAmmo_Implementation(int32 AmmoToAdd)
{
	if (HasAuthority()) return;
	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);
	OwningCharacter = OwningCharacter = nullptr ? Cast<AMagiCharacter>(GetOwner()) : OwningCharacter;
	if (OwningCharacter && OwningCharacter->GetCombat() && IsFull())
	{
		OwningCharacter->GetCombat()->JumpToShotgunEnd();
	}
	SetHUDAmmo();
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	OnWeaponStateSet();
}

void AWeapon::OnRep_WeaponState()
{
	OnWeaponStateSet();
}

void AWeapon::OnWeaponStateSet()
{
	switch (WeaponState)
	{
		case EWeaponState::EWS_Equipped:
			HandleWeaponEquipped();
			break;
		case EWeaponState::EWS_Dropped:
			HandleWeaponDropped();
			break;
		case EWeaponState::EWS_EquippedSecondary:
			HandleSecondaryWeaponEquipped();
			break;
	}
}

void AWeapon::OnPingTooHigh(bool bPingTooHigh)
{
	bUseServerSideRewind = !bPingTooHigh;
}

void AWeapon::HandleWeaponDropped()
{
	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);

	OwningCharacter = OwningCharacter == nullptr ? Cast<AMagiCharacter>(GetOwner()) : OwningCharacter;
	if (OwningCharacter && bUseServerSideRewind)
	{
		OwningController = OwningController == nullptr ? Cast<AMagiPlayerController>(OwningCharacter->Controller) : OwningController;
		if (OwningController && HasAuthority() && OwningController->HighPingDelegate.IsBound())
		{
			OwningController->HighPingDelegate.RemoveDynamic(this, &AWeapon::OnPingTooHigh);
		}
	}
}

void AWeapon::HandleWeaponEquipped()
{
	if (WeaponMesh == nullptr) return;
	ShowPickupWidget(false);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (WeaponType == EWeaponType::EWT_SMG)
	{
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}
	EnableCustomDepth(false);

	OwningCharacter = OwningCharacter == nullptr ? Cast<AMagiCharacter>(GetOwner()) : OwningCharacter;
	if (OwningCharacter && bUseServerSideRewind)
	{
		OwningController = OwningController == nullptr ? Cast<AMagiPlayerController>(OwningCharacter->Controller) : OwningController;
		if (OwningController && HasAuthority() && !OwningController->HighPingDelegate.IsBound())
		{
			OwningController->HighPingDelegate.AddDynamic(this, &AWeapon::OnPingTooHigh);
		}
	}
}

void AWeapon::HandleSecondaryWeaponEquipped()
{
	if (WeaponMesh == nullptr) return;
	ShowPickupWidget(false);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (WeaponType == EWeaponType::EWT_SMG)
	{
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}
	EnableCustomDepth(true);
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);
	WeaponMesh->MarkRenderStateDirty();

	OwningCharacter = OwningCharacter == nullptr ? Cast<AMagiCharacter>(GetOwner()) : OwningCharacter;
	if (OwningCharacter && bUseServerSideRewind)
	{
		OwningController = OwningController == nullptr ? Cast<AMagiPlayerController>(OwningCharacter->Controller) : OwningController;
		if (OwningController && HasAuthority() && !OwningController->HighPingDelegate.IsBound())
		{
			OwningController->HighPingDelegate.AddDynamic(this, &AWeapon::OnPingTooHigh);
		}
	}
}

bool AWeapon::IsEmpty()
{
	return Ammo <= 0;
}

bool AWeapon::IsFull()
{
	return Ammo == MagCapacity;
}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::SetHUDAmmo()
{
	OwningCharacter = OwningCharacter == nullptr ? Cast<AMagiCharacter>(GetOwner()) : OwningCharacter;
	if (OwningCharacter)
	{
		OwningController = OwningController == nullptr ? Cast<AMagiPlayerController>(OwningCharacter->Controller) : OwningController;
		if (OwningController)
		{
			OwningController->SetHUDWeaponAmmo(Ammo);
		}
	}
}

FVector AWeapon::TraceEndWithScatter(const FVector& HitTarget)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket == nullptr) return FVector();

	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector TraceStart = SocketTransform.GetLocation();

	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	const FVector RandVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.0f, SphereRadius);
	const FVector EndLocation = SphereCenter + RandVector;
	const FVector ToEndLocation = EndLocation - TraceStart;

	//DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, true);
	//DrawDebugSphere(GetWorld(), EndLocation, 4.0f, 12, FColor::Green, true);
	//DrawDebugLine(
	//	GetWorld(),
	//	TraceStart, 
	//	FVector(TraceStart + ToEndLocation * TRACE_LENGTH / ToEndLocation.Size()),
	//	FColor::Blue,
	//	true
	//);

	return FVector(TraceStart + ToEndLocation * TRACE_LENGTH / ToEndLocation.Size());
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	if (CasingClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);
			if (UWorld* world = GetWorld())
			{
				FRotator CasingRotation = SocketTransform.GetRotation().Rotator() + GetRandomRotator();
				world->SpawnActor<ACasing>(
					CasingClass,
					SocketTransform.GetLocation(),
					CasingRotation
				);
			}
		}
	}
	SpendRound();
}

void AWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	OwningCharacter = nullptr;
	OwningController = nullptr;
}



