// Fill out your copyright notice in the Description page of Project Settings.


#include "KillFeedSlot.h"

#include "Components/TextBlock.h"

void UKillFeedSlot::SetKillerText(FText IncomingText)
{
	Killer->SetText(IncomingText);
}

void UKillFeedSlot::SetKilledText(FText IncomingText)
{
	Killed->SetText(IncomingText);
}
