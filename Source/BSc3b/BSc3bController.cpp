// Fill out your copyright notice in the Description page of Project Settings.


#include "BSc3bController.h"

#include "BSc3bCharacter.h"
#include "PlayerAnimation.h"
#include "PlayerHUD.h"
#include "Components/Button.h"

ABSc3bController::ABSc3bController()
{
	Player = nullptr;
}

void ABSc3bController::BeginPlay()
{
	Super::BeginPlay();
	//Cast to our character just in case we ever need it
	PlayerClass = Cast<ABSc3bCharacter>(GetCharacter());

	//Set and spawn our HUD onto the client
	if (!IsValid(PlayerHUDClass))
	{
		return;
	}
	if (IsLocalPlayerController())
	{
		PlayerHUD = CreateWidget<UPlayerHUD>(GetWorld(), PlayerHUDClass);
    	PlayerHUD->AddToViewport();	
	}
	//Set our player animation class so we can adjust the values inside of it directly from our character
	if (!IsValid(PlayerAnimClass))
	{
		return;
	}
	PlayerAnim = PlayerAnimClass.GetDefaultObject();
	
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
