// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KillFeedSlot.generated.h"

/**
 * 
 */

class UTextBlock;

UCLASS()
class BSC3B_API UKillFeedSlot : public UUserWidget
{
	GENERATED_BODY()

	/* Player name that shot the bullet */
	UPROPERTY()
	UTextBlock* Killer;

	/* Player name that got killed from the bullet */
	UPROPERTY()
	UTextBlock* Killed;
	
public:

	UFUNCTION()
	void SetKillerText(FText IncomingText);
	
	UFUNCTION()
	void SetKilledText(FText IncomingText);
};
