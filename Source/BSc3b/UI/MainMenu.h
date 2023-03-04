// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class UEditableText;
class UVerticalBox;

/**
 * 
 */
UCLASS()
class BSC3B_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UVerticalBox* ServerBrowser;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UEditableText* ServerPassword;

public:
	UFUNCTION()
	void ShowServerBrowser();

	UFUNCTION()
	void AddSlotToServerBrowser(UUserWidget* IncomingWidget);

	UFUNCTION()
	FString GetServerPassword();

	UFUNCTION()
	void ClearServerBrowser();
};
