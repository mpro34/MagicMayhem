// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "SpeedPickup.generated.h"

/**
 * 
 */
UCLASS()
class MAGICMAYHEM_API ASpeedPickup : public APickup
{
	GENERATED_BODY()
	
public:

protected:
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

private:
	UPROPERTY(EditAnywhere)
	float BaseSpeedBuff = 1600.0f;

	UPROPERTY(EditAnywhere)
	float CrouchSpeedBuff = 850.0f;

	UPROPERTY(EditAnywhere)
	float SpedBuffTime = 30.0f;
};
