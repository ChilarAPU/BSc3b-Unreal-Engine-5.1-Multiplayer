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

void UPlayerHUD::SetButtonVisibility(bool bVisible)
{
	if (bVisible)
	{
		ScopeButton->SetVisibility(ESlateVisibility::Visible);
		LongScopeButton->SetVisibility(ESlateVisibility::Visible);
		SilencerButton->SetVisibility(ESlateVisibility::Visible);
		ForegripButton->SetVisibility(ESlateVisibility::Visible);
	} else
	{
		ScopeButton->SetVisibility(ESlateVisibility::Hidden);
		LongScopeButton->SetVisibility(ESlateVisibility::Hidden);
		SilencerButton->SetVisibility(ESlateVisibility::Hidden);
		ForegripButton->SetVisibility(ESlateVisibility::Hidden);
	}
}
