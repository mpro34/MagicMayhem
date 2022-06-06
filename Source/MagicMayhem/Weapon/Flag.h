// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Flag.generated.h"

/**
 * 
 */
UCLASS()
class MAGICMAYHEM_API AFlag : public AWeapon
{
	GENERATED_BODY()

public:
	AFlag();
	virtual void Dropped() override;

protected:
	virtual void HandleWeaponDropped() override;
	virtual void HandleWeaponEquipped() override;

private:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* FlagMesh;
	
};
