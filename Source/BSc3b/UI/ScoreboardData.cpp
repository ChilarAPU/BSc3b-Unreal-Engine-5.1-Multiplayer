// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreboardData.h"

#include "Components/TextBlock.h"

void UScoreboardData::SetPlayerID(FString IncomingName)
{
	PlayerID->SetText(FText::FromString(IncomingName));
}

void UScoreboardData::SetPlayerKills(int IncomingKills)
{
	TotalPlayerKills->SetText(FText::AsNumber(IncomingKills));
}

void UScoreboardData::SetPlayerDeaths(int IncomingDeaths)
{
	TotalPlayerDeaths->SetText(FText::AsNumber(IncomingDeaths));
}
