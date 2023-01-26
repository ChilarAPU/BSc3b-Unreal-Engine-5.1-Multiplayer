// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */

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

	UPROPERTY()
	ABSc3bCharacter* OwningPlayer;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString HealthText;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Health;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* Scope;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* LongScope;

	UFUNCTION()
	void SetButtonVisibility(bool bVisible);
};
