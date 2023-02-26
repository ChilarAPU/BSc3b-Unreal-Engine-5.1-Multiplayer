// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuGameState.h"
#include "../UI/GlobalHUD.h"

UGlobalHUD* AMenuGameState::GetGlobalWidget()
{
	return ClientOnlyWidget;
}

void AMenuGameState::BeginPlay()
{
	Super::BeginPlay();
	if (!IsValid(ClientOnlyWidgetClass))
	{
		return;
	}
	ClientOnlyWidget = CreateWidget<UGlobalHUD>(GetWorld(), ClientOnlyWidgetClass);
	ClientOnlyWidget->AddToViewport();
}
