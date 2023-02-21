// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuGameState.h"

#include "GlobalHUD.h"
#include "Net/UnrealNetwork.h"

void AMenuGameState::ChangeServerName()
{
	ServerName = TEXT("Main Session");
}

void AMenuGameState::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("WW"));	
	}
	if (!IsValid(ClientOnlyWidgetClass))
	{
		return;
	}
	ClientOnlyWidget = CreateWidget<UGlobalHUD>(GetWorld(), ClientOnlyWidgetClass);
	ClientOnlyWidget->AddToViewport();
}

void AMenuGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMenuGameState, ServerName);
}
