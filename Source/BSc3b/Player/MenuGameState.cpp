// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuGameState.h"

#include "BSc3bController.h"
#include "../UI/GlobalHUD.h"
#include "BSc3b/EOS_GameInstance.h"
#include "Kismet/GameplayStatics.h"

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

void AMenuGameState::Multicast_EndSession_Implementation(const FString& WinningPlayerID)
{
	ABSc3bController* PC = Cast<ABSc3bController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	PC->ShowGameOverMessage(WinningPlayerID);
	PC->SetCinematicMode(true, true, true); //Stop player movement
	FTimerHandle GameOverHandle;
	GetWorld()->GetTimerManager().SetTimer(GameOverHandle, FTimerDelegate::CreateUObject(this, &AMenuGameState::EndSession), 10.f, false);
}

void AMenuGameState::EndSession()
{
	UEOS_GameInstance* GI = Cast<UEOS_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ABSc3bController* PC = Cast<ABSc3bController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	PC->SetCinematicMode(false, false, false); //Reset back to normal input just in case
	if (HasAuthority()) //Destroy session only as server
	{
		GI->DestroySession();
	}
	GI->ReturnToMainMenu(); //Return all players back to main menu
}
