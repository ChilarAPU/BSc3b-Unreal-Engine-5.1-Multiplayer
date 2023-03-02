// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatistics.h"

#include "Net/UnrealNetwork.h"

APlayerStatistics::APlayerStatistics()
{
	PlayerDeaths = 0;
	PlayerKills = 0;
}

void APlayerStatistics::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerStatistics, PlayerKills);
	DOREPLIFETIME(APlayerStatistics, PlayerDeaths);
	DOREPLIFETIME(APlayerStatistics, PlayerEpicID);
}

void APlayerStatistics::AddKillToScore(int Amount)
{
	PlayerKills = PlayerKills + Amount;
}

void APlayerStatistics::AddDeathToScore(int Amount)
{
	PlayerDeaths = PlayerDeaths + Amount;
}

void APlayerStatistics::SetPlayerEpicID(FString IncomingID)
{
	PlayerEpicID = IncomingID;
}

int APlayerStatistics::GetTotalPlayerKills()
{
	return PlayerKills;
}

int APlayerStatistics::GetTotalPlayerDeath()
{
	return PlayerDeaths;
}

FString APlayerStatistics::GetPlayerEpicID()
{
	return PlayerEpicID;
}
