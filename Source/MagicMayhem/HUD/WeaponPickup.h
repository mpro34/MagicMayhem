// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponPickup.generated.h"

/**
 * 
 */
UCLASS()
class MAGICMAYHEM_API UWeaponPickup : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetWeaponImage(class UTexture2D* WeaponTexture);

	void SetAmmoAmtText(int32 AmmoAmt);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AmmoAmtText;

	UPROPERTY(meta = (BindWidget))
	class UImage* WeaponImage;
};
