// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuGameState.h"
#include "Net/UnrealNetwork.h"

void AMenuGameState::ChangeServerName()
{
	ServerName = TEXT("Main Session");
}

void AMenuGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMenuGameState, ServerName);
}
