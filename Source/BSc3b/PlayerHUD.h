// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */

class UButton;
class UTextBlock;
class UProgressBar;

class ABSc3bCharacter;

UCLASS()
class BSC3B_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnScopeClicked();

	UFUNCTION()
	void OnLongScopeClicked();

	UFUNCTION()
	void OnSilencerClicked();

	UFUNCTION()
	void OnForegripClicked();

	UFUNCTION()
	void OnRespawnClicked();

	UPROPERTY()
	ABSc3bCharacter* OwningPlayer;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UTextBlock* MobilityText;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UTextBlock* StabilityText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UTextBlock* RangeText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UTextBlock* DamageText;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString HealthText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AmmoCount;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Health;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* ScopeButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* LongScopeButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* SilencerButton;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* ForegripButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* RespawnButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* DamageStatBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* RangeStatBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* StabilityStatBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* MobilityStatBar;
	
	UFUNCTION()
	void AdjustStatPercentage(UProgressBar* Bar, float value);

	UFUNCTION()
	void SetButtonVisibility(bool bVisible);
};
