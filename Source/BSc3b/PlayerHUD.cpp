// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"

#include "BSc3bCharacter.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
//Include weapons header file so we can access the enums
#include "Weapon.h"
#include "Components/Button.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();
	Scope->OnClicked.AddDynamic(this, &UPlayerHUD::OnScopeClicked);
	Scope->IsFocusable = false;
	
	LongScope->OnClicked.AddDynamic(this, &UPlayerHUD::OnLongScopeClicked);
	LongScope->IsFocusable = false;
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

	OwningPlayer->EquipWeaponAttachment(ForeGrip);
	OwningPlayer->Server_EquipWeaponAttachment(ForeGrip);
}

void UPlayerHUD::SetButtonVisibility(bool bVisible)
{
	if (bVisible)
	{
		Scope->SetVisibility(ESlateVisibility::Visible);
		LongScope->SetVisibility(ESlateVisibility::Visible);
	} else
	{
		Scope->SetVisibility(ESlateVisibility::Hidden);
		LongScope->SetVisibility(ESlateVisibility::Hidden);
	}
}
