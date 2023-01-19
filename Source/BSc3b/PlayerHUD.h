// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */

UCLASS()
class BSC3B_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString HealthText;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Health;
};
