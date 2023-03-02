// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MenuGameState.generated.h"

class UGlobalHUD;
class APlayerState;
/**
 * 
 */

USTRUCT()
struct FScoreboardData
{
	GENERATED_BODY()

	FScoreboardData()
	{
		Name = TEXT("Default Value");
		TotalKills = 0;
		TotalDeaths = 0;
	}

	UPROPERTY()
	FString Name;

	UPROPERTY()
	int TotalKills;

	UPROPERTY()
	int TotalDeaths;
};

UCLASS()
class BSC3B_API AMenuGameState : public AGameStateBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess))
	TSubclassOf<UGlobalHUD> ClientOnlyWidgetClass;

	/* Widget that is replicated to all clients screens at the same time */
	UPROPERTY()
	UGlobalHUD* ClientOnlyWidget;

	UPROPERTY()
	TMap<APlayerState*, FScoreboardData> Scoreboard;

public:
	
	UFUNCTION()
	UGlobalHUD* GetGlobalWidget();

	virtual void BeginPlay() override;
	
};
