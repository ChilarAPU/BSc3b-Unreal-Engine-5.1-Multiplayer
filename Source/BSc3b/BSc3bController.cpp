// Fill out your copyright notice in the Description page of Project Settings.


#include "BSc3bController.h"

#include "BSc3bCharacter.h"
#include "PlayerHUD.h"
#include "Components/Button.h"

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

void ABSc3bController::ShowRespawnButton(bool Visible)
{
	if (IsValid(PlayerHUD))
	{
		SetShowMouseCursor(Visible);
		SetIgnoreLookInput(Visible);
		if (Visible)
		{
			PlayerHUD->RespawnButton->SetVisibility(ESlateVisibility::Visible);
			const FInputModeUIOnly Input;
			SetInputMode(Input);
		}
		else
		{
			PlayerHUD->RespawnButton->SetVisibility(ESlateVisibility::Hidden);
			const FInputModeGameOnly Input;
			SetInputMode(Input);
		}
		
	}
	
}
