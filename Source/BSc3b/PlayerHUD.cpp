// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"

#include "BSc3bCharacter.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
//Include weapons header file so we can access the enums
#include "Weapon.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();
	ScopeButton->OnClicked.AddDynamic(this, &UPlayerHUD::OnScopeClicked);
	ScopeButton->IsFocusable = false;
	
	LongScopeButton->OnClicked.AddDynamic(this, &UPlayerHUD::OnLongScopeClicked);
	LongScopeButton->IsFocusable = false;

	SilencerButton->OnClicked.AddDynamic(this, &UPlayerHUD::OnSilencerClicked);
	SilencerButton->IsFocusable = false;

	ForegripButton->OnClicked.AddDynamic(this, &UPlayerHUD::OnForegripClicked);
	ForegripButton->IsFocusable = false;

	RespawnButton->OnClicked.AddDynamic(this, &UPlayerHUD::OnRespawnClicked);
	RespawnButton->IsFocusable = false;

}

void UPlayerHUD::OnScopeClicked()
{
	//Set our owning player
	//Need to do this on click otherwise we could be pointing to a previous player pawn
	OwningPlayer = Cast<ABSc3bCharacter>(GetOwningPlayerPawn());

	OwningPlayer->EquipWeaponAttachment(RedDot);
	OwningPlayer->Server_EquipWeaponAttachment(RedDot);
}

void UPlayerHUD::OnLongScopeClicked()
{
	OwningPlayer = Cast<ABSc3bCharacter>(GetOwningPlayerPawn());

	OwningPlayer->EquipWeaponAttachment(LongRange);
	OwningPlayer->Server_EquipWeaponAttachment(LongRange);
}

void UPlayerHUD::OnSilencerClicked()
{
	OwningPlayer = Cast<ABSc3bCharacter>(GetOwningPlayerPawn());

	OwningPlayer->EquipWeaponAttachment(Silencer);
	OwningPlayer->Server_EquipWeaponAttachment(Silencer);
}

void UPlayerHUD::OnForegripClicked()
{
	OwningPlayer = Cast<ABSc3bCharacter>(GetOwningPlayerPawn());

	OwningPlayer->EquipWeaponAttachment(ForeGrip);
	OwningPlayer->Server_EquipWeaponAttachment(ForeGrip);
}

void UPlayerHUD::OnRespawnClicked()
{
	OwningPlayer = Cast<ABSc3bCharacter>(GetOwningPlayerPawn());
	OwningPlayer->Server_Respawn();
}

void UPlayerHUD::AdjustStatPercentage(UProgressBar* Bar, float value)
{
	//divide our value by 10 as the max it can be is 10, but needs to be between 0-1 in the progress bar
	value = value / 10;
	Bar->SetPercent(value);
}

void UPlayerHUD::SetButtonVisibility(bool bVisible)
{
	if (bVisible)
	{
		ScopeButton->SetVisibility(ESlateVisibility::Visible);
		LongScopeButton->SetVisibility(ESlateVisibility::Visible);
		SilencerButton->SetVisibility(ESlateVisibility::Visible);
		ForegripButton->SetVisibility(ESlateVisibility::Visible);
		DamageStatBar->SetVisibility(ESlateVisibility::Visible);
		DamageText->SetVisibility(ESlateVisibility::Visible);
		RangeStatBar->SetVisibility(ESlateVisibility::Visible);
		RangeText->SetVisibility(ESlateVisibility::Visible);
		StabilityStatBar->SetVisibility(ESlateVisibility::Visible);
		StabilityText->SetVisibility(ESlateVisibility::Visible);
		MobilityStatBar->SetVisibility(ESlateVisibility::Visible);
		MobilityText->SetVisibility(ESlateVisibility::Visible);
	} else
	{
		ScopeButton->SetVisibility(ESlateVisibility::Hidden);
		LongScopeButton->SetVisibility(ESlateVisibility::Hidden);
		SilencerButton->SetVisibility(ESlateVisibility::Hidden);
		ForegripButton->SetVisibility(ESlateVisibility::Hidden);
		DamageStatBar->SetVisibility(ESlateVisibility::Hidden);
		DamageText->SetVisibility(ESlateVisibility::Hidden);
		RangeStatBar->SetVisibility(ESlateVisibility::Hidden);
		RangeText->SetVisibility(ESlateVisibility::Hidden);
		StabilityStatBar->SetVisibility(ESlateVisibility::Hidden);
		StabilityText->SetVisibility(ESlateVisibility::Hidden);
		MobilityStatBar->SetVisibility(ESlateVisibility::Hidden);
		MobilityText->SetVisibility(ESlateVisibility::Hidden);
	}
}
