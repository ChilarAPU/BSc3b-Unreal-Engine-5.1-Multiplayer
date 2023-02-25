// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GlobalHUD.h"
#include "ChatBox.generated.h"

/**
 * 
 */

class UTextBlock;
class UMultiLineEditableTextBox;

UCLASS()
class BSC3B_API UChatBox : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UTextBlock* Time;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UTextBlock* PlayerID;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UMultiLineEditableTextBox* Message;

public:
	UFUNCTION()
	void SetChatMessage(FCustomChatMessage IncomingMessage);
	
};
