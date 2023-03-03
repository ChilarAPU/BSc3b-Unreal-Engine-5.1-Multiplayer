// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerSlot.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class BSC3B_API UServerSlot : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UTextBlock* ServerName;

public:

	UFUNCTION()
	void SetServerName(FString IncomingName);

	UPROPERTY(BlueprintReadWrite)
	int PlaceInSearchResult;
};
