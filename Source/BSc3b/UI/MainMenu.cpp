// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"

#include "Components/Border.h"
#include "Components/EditableText.h"
#include "Components/VerticalBox.h"

void UMainMenu::ShowServerBrowser()
{
	ServerBrowser->SetVisibility(ESlateVisibility::Visible);
	ServerBrowserHeader->SetVisibility(ESlateVisibility::Visible);
}

void UMainMenu::AddSlotToServerBrowser(UUserWidget* IncomingWidget)
{
	UE_LOG(LogTemp, Warning, TEXT("Slot Added"));
	ServerBrowser->AddChildToVerticalBox(IncomingWidget);
}

FString UMainMenu::GetServerPassword()
{
	return FString(ServerPassword->GetText().ToString());
}

void UMainMenu::ClearServerBrowser()
{
	ServerBrowser->ClearChildren();
}
