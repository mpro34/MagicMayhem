// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponPickup.h"
#include "Engine/Texture2D.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UWeaponPickup::SetWeaponImage(UTexture2D* WeaponTexture)
{
	if (WeaponImage)
	{
		WeaponImage->SetBrushFromTexture(WeaponTexture, true);
	}
}

void UWeaponPickup::SetAmmoAmtText(int32 AmmoAmt)
{
	FString AmmoAmtString = FString::Printf(TEXT("%d"), AmmoAmt);
	if (AmmoAmtText)
	{
		AmmoAmtText->SetText(FText::FromString(AmmoAmtString));
	}
}