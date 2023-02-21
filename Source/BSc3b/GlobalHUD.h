// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GlobalHUD.generated.h"

/**
 * 
 */
class UTextBlock;
class UUniformGridPanel;
class UKillFeedSlot;

UCLASS()
class BSC3B_API UGlobalHUD : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess))
	TSubclassOf<UKillFeedSlot> KillFeedWidgetClass;

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UTextBlock* MessageTextBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UUniformGridPanel* KillFeedBox;

	UPROPERTY(BlueprintReadOnly)
	UKillFeedSlot* KillFeedWidget;

	UFUNCTION()
	void AddToKilLFeed(const FString& HitPlayerName, const FString& ShootingPlayerName);
	
};
