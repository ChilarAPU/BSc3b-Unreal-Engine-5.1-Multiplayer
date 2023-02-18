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
class UImage;

class ABSc3bCharacter;

UCLASS()
class BSC3B_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

	////// UMG BUTTON DELEGATE FUNCTIONS //////
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

	////// UMG TEXT VALUES ONLY BOUND IN C++ TO ADJUST VISIBILITY //////
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UTextBlock* MobilityText;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UTextBlock* StabilityText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UTextBlock* RangeText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UTextBlock* DamageText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UProgressBar* PlayerHealth;

public:

	////// UMG WIDGETS BOUND TO BLUEPRINT VALUES //////
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AmmoCount;

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

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* HitMarker;
	
	/* Divide our statistic values so they work in the 0 - 1 value range which is required for
	 * progress bars to function correctly
	 */
	UFUNCTION()
	void AdjustStatPercentage(UProgressBar* Bar, float value);

	/* Adjust the visibility of everything relates to attachments in the widget */
	UFUNCTION()
	void SetButtonVisibility(bool bVisible);

	UFUNCTION()
	void ShowHitmarker();

	UFUNCTION()
	void HideHitmarker();

	UFUNCTION()
	void AdjustPlayerHealthBar(float Value);

	UPROPERTY()
	FTimerHandle VisibilityHandler;
};
