// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalHUD.h"

#include "BSc3bCharacter.h"
#include "EOS_GameInstance.h"
#include "KillFeedSlot.h"
#include "Components/Button.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetTextLibrary.h"
#include "ChatBox.h"
#include "Components/VerticalBox.h"

void UGlobalHUD::NativeConstruct()
{
	Super::NativeConstruct();
	SendMessageButton->OnClicked.AddDynamic(this, &UGlobalHUD::SendMessageButtonOnPressed);
	SendMessageButton->IsFocusable = false;
}

void UGlobalHUD::AddToKilLFeed(const FString& HitPlayerName, const FString& ShootingPlayerName)
{
	if (KillFeedWidgetClass)
	{
		KillFeedWidget = CreateWidget<UKillFeedSlot>(GetWorld(), KillFeedWidgetClass);
		KillFeedWidget->Killed->SetText(FText::FromString(HitPlayerName));
		KillFeedWidget->Killer->SetText(FText::FromString(ShootingPlayerName));
		int32 ChildrenInKillFeed = KillFeedBox->GetChildrenCount();
		UUniformGridSlot* FeedSlot = KillFeedBox->AddChildToUniformGrid(KillFeedWidget, ChildrenInKillFeed);
		FeedSlot->SetHorizontalAlignment(HAlign_Fill);
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
		FCustomChatMessage CurrentMessage = FCustomChatMessage(MessageLoc, FormattedTime, GI->PlayerName);
		ABSc3bCharacter* PawnRef = Cast<ABSc3bCharacter>(GetOwningPlayerPawn());
		PawnRef->Server_ReceiveMessage(CurrentMessage);
		ClearChatBox();
	}
}

void UGlobalHUD::SendMessageToBox(FCustomChatMessage Message)
{
	if (ChatBoxWidgetClass)
	{
		ChatBoxWidget = CreateWidget<UChatBox>(GetWorld(), ChatBoxWidgetClass);
		//Call ChatBox function to add child to vertical box, passing through the message
		ChatBoxWidget->SetChatMessage(Message);
		AllChannelMessages->AddChildToVerticalBox(ChatBoxWidget);
	}
}

void UGlobalHUD::ClearChatBox()
{
	MessageToSend->SetText(FText::GetEmpty());
}
