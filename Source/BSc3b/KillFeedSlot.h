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
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	UTextBlock* Killer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	UTextBlock* Killed;
};
