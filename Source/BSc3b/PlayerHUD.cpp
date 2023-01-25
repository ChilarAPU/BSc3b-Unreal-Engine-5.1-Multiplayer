// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();
	Scope->OnClicked.AddDynamic(this, &UPlayerHUD::OnScopeClicked);
	Scope->IsFocusable = false;
}

void UPlayerHUD::OnScopeClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("WW"));
}

void UPlayerHUD::SetButtonVisibility(bool bVisible)
{
	if (bVisible)
	{
		Scope->SetVisibility(ESlateVisibility::Visible);
	} else
	{
		Scope->SetVisibility(ESlateVisibility::Hidden);
	}
}
