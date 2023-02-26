// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerStatistics.generated.h"

/**
 * 
 */
UCLASS()
class BSC3B_API APlayerStatistics : public APlayerState
{
	GENERATED_BODY()

	APlayerStatistics();

	UPROPERTY(Replicated)
	int PlayerKills;
	
	UPROPERTY(Replicated)
	int PlayerDeaths;

	/* All replicated values must be called in this function with any conditions */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION()
	void AddKillToScore(int Amount);

	UFUNCTION()
	void AddDeathToScore(int Amount);

	UFUNCTION()
	int GetTotalPlayerKills();

	UFUNCTION()
	int GetTotalPlayerDeath();
};
