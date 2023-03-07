// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatistics.h"

#include "MenuGameState.h"
#include "BSc3b/BSc3bGameMode.h"
#include "BSc3b/EOS_GameInstance.h"
#include "Kismet/GameplayStatics.h"
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
	//We are always on the server at this point
	ABSc3bGameMode* GameModeRef = Cast<ABSc3bGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	PlayerKills = PlayerKills + Amount;
	if (!GameModeRef)
	{
		return;
	}
	AMenuGameState* GS = Cast<AMenuGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (PlayerKills >= GameModeRef->GetTargetNumberOfKills())
	{
		GS->Multicast_EndSession(GetPlayerEpicID());
		//GI->DestroySession();
		//GI->ReturnToMainMenu();
	}
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
