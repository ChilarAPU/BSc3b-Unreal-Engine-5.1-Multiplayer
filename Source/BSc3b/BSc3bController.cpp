// Fill out your copyright notice in the Description page of Project Settings.


#include "BSc3bController.h"

#include "BSc3bCharacter.h"
#include "PlayerHUD.h"

ABSc3bController::ABSc3bController()
{
	Player = nullptr;
}

void ABSc3bController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Controller"));
	PlayerClass = Cast<ABSc3bCharacter>(GetCharacter());
	
	if (!IsValid(PlayerHUDClass))
	{
		return;
	}
	if (IsLocalPlayerController())
	{
		PlayerHUD = CreateWidget<UPlayerHUD>(GetWorld(), PlayerHUDClass);
    	PlayerHUD->AddToViewport();	
	}
	
}
