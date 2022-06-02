// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldPickup.h"
#include "MagiCharacter.h"
#include "MagicMayhem/MagiComponents/BuffComponent.h"

void AShieldPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AMagiCharacter* MagiCharacter = Cast<AMagiCharacter>(OtherActor);
	if (MagiCharacter && MagiCharacter->GetCombat())
	{
		UBuffComponent* Buff = MagiCharacter->GetBuff();
		if (Buff)
		{
			Buff->ReplenishShield(ShieldReplenishAmount, ShieldReplenishTime);
		}
	}
	Destroy();
}
