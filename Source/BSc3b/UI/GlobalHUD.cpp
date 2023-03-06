// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalHUD.h"

#include "../Player/BSc3bCharacter.h"
#include "../Player/BSc3bController.h"
#include "../EOS_GameInstance.h"
#include "KillFeedSlot.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetTextLibrary.h"
#include "ChatBox.h"
#include "ScoreboardData.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "BSc3b/Player/MenuGameState.h"
#include "BSc3b/Player/PlayerStatistics.h"
#include "Components/Border.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Kismet/KismetInputLibrary.h"

void UGlobalHUD::NativeConstruct()
{
	Super::NativeConstruct();
	bIsCurrentlyTyping = false;
}

void UGlobalHUD::ReachedMaximumKillFeedSlots()
{
	NewKilLFeedBox->RemoveChildAt(0); //Remove first row kill feed
	// Move each widget up a row inside the grid panel
	for (int i = 0; i < TotalKillFeedSlots.Num(); i++)
	{
		int32 CurrentRow = TotalKillFeedSlots[i]->GetRow();
		TotalKillFeedSlots[i]->SetRow(CurrentRow - 1);
	}
	TotalKillFeedSlots.RemoveAt(0); //Remove the kill feed widget from the array
}

void UGlobalHUD::SetTimerWithDelegate(FTimerHandle& TimerHandle, TBaseDelegate<void> ObjectDelegate, float Time,
	bool bLoop, UObject* WorldContextObject)
{
	WorldContextObject->GetWorld()->GetTimerManager().ClearTimer(TimerHandle); //if the inputted timer is currently running, then clear it
	WorldContextObject->GetWorld()->GetTimerManager().SetTimer(TimerHandle, ObjectDelegate, Time, bLoop);
}

void UGlobalHUD::RemoveSlotFromKillFeed(UKillFeedSlot* IncomingKillFeedSlot, UGridSlot* IncomingSlot)
{
	//if the slot has already been destroyed automatically by ReachedMaximumKillFeedSlots(), do nothing
	if (!IsValid(IncomingKillFeedSlot))
	{
		return;
	}
	
	NewKilLFeedBox->RemoveChild(IncomingKillFeedSlot); //Remove widget from grid panel
	TotalKillFeedSlots.Remove(IncomingSlot); //Remove widget from kill feed array
}

void UGlobalHUD::HideChatBox()
{
	if (!bIsCurrentlyTyping) //Check that the current client is not typing
	{
		ChatBoxUI->SetVisibility(ESlateVisibility::Hidden);
	}
	
}

void UGlobalHUD::SetConnectMessage(FText IncomingMessage)
{
	MessageTextBox->SetText(IncomingMessage);
	SetTimerWithDelegate(ConnectMessageHandle, FTimerDelegate::CreateUObject(this, &UGlobalHUD::SetConnectMessageVisibility, false),
		2.f, false, GetWorld());
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
	ChatBoxUI->SetVisibility(ESlateVisibility::Visible);
	bIsCurrentlyTyping = true;
	
}

void UGlobalHUD::AddToKilLFeed(const FString& HitPlayerName, const FString& ShootingPlayerName)
{
	if (KillFeedWidgetClass)
	{
		//Setup KillFeed widget
		KillFeedWidget = CreateWidget<UKillFeedSlot>(GetWorld(), KillFeedWidgetClass);
		KillFeedWidget->SetKilledText(FText::FromString(HitPlayerName));
		KillFeedWidget->SetKillerText(FText::FromString(ShootingPlayerName));

		/* Add kill widget to killfeed*/
		const int32 ChildrenInKillFeed = NewKilLFeedBox->GetChildrenCount();
		UGridSlot* NewFeedSlot = NewKilLFeedBox->AddChildToGrid(KillFeedWidget, ChildrenInKillFeed);
		TotalKillFeedSlots.Emplace(NewFeedSlot); //Add UGridSlot to array
		NewFeedSlot->SetHorizontalAlignment(HAlign_Fill);
		
		if (ChildrenInKillFeed >= 2) //Maximum number of kill feed widgets to display on screen
		{
			ReachedMaximumKillFeedSlots();
		}

		//Timer to hide kill feed slot. We do not want this value to be overridable from subsequent calls
		FTimerHandle KillSlotHandle;
		SetTimerWithDelegate(KillSlotHandle, FTimerDelegate::CreateUObject(this, &UGlobalHUD::RemoveSlotFromKillFeed,
			KillFeedWidget, NewFeedSlot), 8.f, false, GetWorld());
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
	} else //If we have decided not to send a message, make sure that the chat box becomes hidden on the client
	{
		SetTimerWithDelegate(ChatMessageHandle, FTimerDelegate::CreateUObject(this, &UGlobalHUD::HideChatBox), 3.f,
			false, GetWorld());
	}
}

void UGlobalHUD::SendMessageToBox(FCustomChatMessage Message)
{
	ChatBoxUI->SetVisibility(ESlateVisibility::Visible);
	if (ChatBoxWidgetClass)
	{
		ChatBoxWidget = CreateWidget<UChatBox>(GetWorld(), ChatBoxWidgetClass);
		//Call ChatBox function to set struct to its associating values in the widget
		ChatBoxWidget->SetChatMessage(Message);
		AllChannelMessages->AddChildToVerticalBox(ChatBoxWidget);
	}
	//Play sound on owning client when a message has been recieved
	ABSc3bCharacter* PawnRef = Cast<ABSc3bCharacter>(GetOwningPlayerPawn());
	if (PawnRef->IsLocallyControlled() && MessageRecieveSound) //make sure we are only running this on the clients machine
	{
		UGameplayStatics::PlaySound2D(GetWorld(), MessageRecieveSound);
	}

	//Hide all chat boxes once a message has been sent
	SetTimerWithDelegate(ChatMessageHandle, FTimerDelegate::CreateUObject(this, &UGlobalHUD::HideChatBox), 3.f, false, GetWorld());
	
}

void UGlobalHUD::ClearChatBox()
{
	MessageToSend->SetText(FText::GetEmpty());
}

void UGlobalHUD::ShowScoreboard(bool bVisible, AMenuGameState* GS)
{
	if (bVisible)
	{
		Scoreboard->SetVisibility(ESlateVisibility::Visible);
		ScoreboardHeader->SetVisibility(ESlateVisibility::Visible);
		//Fill up scoreboard with data
		if (ScoreboardWidgetClass)
		{
			for (int i = 0; i < GS->PlayerArray.Num(); i++)
			{
				ScoreboardWidget = CreateWidget<UScoreboardData>(GetWorld(), ScoreboardWidgetClass);
				APlayerStatistics* PS = Cast<APlayerStatistics>(GS->PlayerArray[i]);
				ScoreboardWidget->SetPlayerID(PS->GetPlayerEpicID());
				ScoreboardWidget->SetPlayerKills(PS->GetTotalPlayerKills());
				ScoreboardWidget->SetPlayerDeaths(PS->GetTotalPlayerDeath());
				Scoreboard->AddChildToVerticalBox(ScoreboardWidget);
			}
		}
		
	} else
	{
		Scoreboard->SetVisibility(ESlateVisibility::Hidden);
		ScoreboardHeader->SetVisibility(ESlateVisibility::Hidden);
		if (ScoreboardWidget)
		{
			Scoreboard->ClearChildren();
		}
		
	}
}

FReply UGlobalHUD::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Input = UKismetInputLibrary::GetKey(InKeyEvent);
	ABSc3bCharacter* PawnRef = Cast<ABSc3bCharacter>(GetOwningPlayerPawn());
	if (UKismetInputLibrary::EqualEqual_KeyKey(Input, EKeys::Enter))  //Check we are using the enter key
	{
		SendMessageButtonOnPressed();  //Replcement for the button delegate
		bIsCurrentlyTyping = false;
		//Restore normal Gameplay to client
		UWidgetBlueprintLibrary::SetFocusToGameViewport();  
		PawnRef->GetActivePlayerController()->SetIgnoreLookInput(false);
	} else if (PawnRef->IsLocallyControlled() && ChatTypingSound)
	{
		float KeyPitch = UKismetMathLibrary::RandomFloatInRange(.7, 1.5);
		UGameplayStatics::PlaySound2D(GetWorld(), ChatTypingSound, 1, KeyPitch);
	}
	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}
