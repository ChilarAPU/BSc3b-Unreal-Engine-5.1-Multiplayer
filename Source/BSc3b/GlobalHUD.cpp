// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalHUD.h"

#include "KillFeedSlot.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

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
