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

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UTextBlock* Ping;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UTextBlock* CurrentPlayerNumber;

public:

	UFUNCTION()
	void SetServerName(FString IncomingName);

	UFUNCTION()
	void SetServerPing(int32 IncomingPing);

	UFUNCTION()
	void SetTotalCurrentPlayersInServer(int32 IncomingNumber, int32 MaxIncomingNumber);
	
	UPROPERTY(BlueprintReadWrite)
	int PlaceInSearchResult;
};
