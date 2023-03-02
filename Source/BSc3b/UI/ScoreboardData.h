// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreboardData.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class BSC3B_API UScoreboardData : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UTextBlock* PlayerID;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UTextBlock* TotalPlayerKills;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UTextBlock* TotalPlayerDeaths;

public:
	UFUNCTION()
	void SetPlayerID(FString IncomingName);

	UFUNCTION()
	void SetPlayerKills(int IncomingKills);

	UFUNCTION()
	void SetPlayerDeaths(int IncomingDeaths);
	
};
