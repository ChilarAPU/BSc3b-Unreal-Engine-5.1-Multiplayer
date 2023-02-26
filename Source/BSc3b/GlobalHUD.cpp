// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalHUD.h"

#include "BSc3bCharacter.h"
#include "BSc3bController.h"
#include "EOS_GameInstance.h"
#include "KillFeedSlot.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetTextLibrary.h"
#include "ChatBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/VerticalBox.h"
#include "Kismet/KismetInputLibrary.h"

void UGlobalHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGlobalHUD::SetConnectMessage(FText IncomingMessage)
{
	MessageTextBox->SetText(IncomingMessage);
}

void UGlobalHUD::SetConnectMessageVisibility(bool bShouldBeVisible)
{
	if (bShouldBeVisible)
	{
		MessageTextBox->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		MessageTextBox->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGlobalHUD::SetFocusToTextBox(APlayerController* PlayerController)
{
	MessageToSend->SetUserFocus(PlayerController);
}

void UGlobalHUD::AddToKilLFeed(const FString& HitPlayerName, const FString& ShootingPlayerName)
{
	if (KillFeedWidgetClass)
	{
		//Setup KillFeed widget
		KillFeedWidget = CreateWidget<UKillFeedSlot>(GetWorld(), KillFeedWidgetClass);
		KillFeedWidget->SetKilledText(FText::FromString(HitPlayerName));
		KillFeedWidget->SetKillerText(FText::FromString(ShootingPlayerName));
		
		//Add widget to UniformGridPanel
		int32 ChildrenInKillFeed = KillFeedBox->GetChildrenCount();
		UUniformGridSlot* FeedSlot = KillFeedBox->AddChildToUniformGrid(KillFeedWidget, ChildrenInKillFeed);
		FeedSlot->SetHorizontalAlignment(HAlign_Fill);
		
		// TODO: Remove kill feed widget after some time will still adding in to the grid panel correctly
	}
}

void UGlobalHUD::SendMessageButtonOnPressed()
{
	//Get the inputted text and trim any dead space
	FText MessageLoc = UKismetTextLibrary::TextTrimPrecedingAndTrailing(MessageToSend->GetText());
	
	FDateTime Time = UKismetMathLibrary::UtcNow();  //Access the date and time from the owners machine

	//Append time to a usable string 
	TArray<FStringFormatArg> args;
	args.Add(Time.GetHour());
	args.Add(Time.GetMinute());
	FString FormattedTime = FString::Format(TEXT("{0}:{1}"), args);
	
	if (!UKismetTextLibrary::TextIsEmpty(MessageLoc))  //Make sure we are not sending an empty message
	{
		// Game instance ref so we can access our clients epic ID
		UEOS_GameInstance* GI = Cast<UEOS_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		
		//Current Message struct filled out
		FCustomChatMessage CurrentMessage = FCustomChatMessage(MessageLoc, FormattedTime, GI->GetPlayerEpicID());
		
		ABSc3bCharacter* PawnRef = Cast<ABSc3bCharacter>(GetOwningPlayerPawn());
		PawnRef->Server_ReceiveMessage(CurrentMessage);  //Send message to all clients
		ClearChatBox();
	}
}

void UGlobalHUD::SendMessageToBox(FCustomChatMessage Message)
{
	if (ChatBoxWidgetClass)
	{
		ChatBoxWidget = CreateWidget<UChatBox>(GetWorld(), ChatBoxWidgetClass);
		//Call ChatBox function to set struct to its associating values in the widget
		ChatBoxWidget->SetChatMessage(Message);
		AllChannelMessages->AddChildToVerticalBox(ChatBoxWidget);
	}
}

void UGlobalHUD::ClearChatBox()
{
	MessageToSend->SetText(FText::GetEmpty());
}

FReply UGlobalHUD::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Input = UKismetInputLibrary::GetKey(InKeyEvent);
	if (UKismetInputLibrary::EqualEqual_KeyKey(Input, EKeys::Enter))  //Check we are using the enter key
	{
		SendMessageButtonOnPressed();  //Replcement for the button delegate
		
		//Restore normal Gameplay to client
		UWidgetBlueprintLibrary::SetFocusToGameViewport();  
		ABSc3bCharacter* PawnRef = Cast<ABSc3bCharacter>(GetOwningPlayerPawn());
		PawnRef->GetActivePlayerController()->SetIgnoreLookInput(false);
	}
	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}
