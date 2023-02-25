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
class UVerticalBox;
class UMultiLineEditableTextBox;
class UChatBox;

USTRUCT(BlueprintType)
struct FCustomChatMessage
{
	GENERATED_BODY()

	FCustomChatMessage()
	{
		Message = FText::GetEmpty();
		TimeOfMessage = TEXT("00:00");
		PlayerID = TEXT("Default");
	}

	FCustomChatMessage(FText MessageLoc, FString Time, FString ID)
	{
		Message = MessageLoc;
		TimeOfMessage = Time;
		PlayerID = ID;
	}

	UPROPERTY()
	FText Message;

	UPROPERTY()
	FString TimeOfMessage;

	UPROPERTY()
	FString PlayerID;
};

UCLASS()
class BSC3B_API UGlobalHUD : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess))
	TSubclassOf<UKillFeedSlot> KillFeedWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess))
	TSubclassOf<UChatBox> ChatBoxWidgetClass;

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UTextBlock* MessageTextBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UUniformGridPanel* KillFeedBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UVerticalBox* AllChannelMessages;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UMultiLineEditableTextBox* MessageToSend;

	UPROPERTY(BlueprintReadOnly)
	UKillFeedSlot* KillFeedWidget;

	UPROPERTY(BlueprintReadOnly)
	UChatBox* ChatBoxWidget;


	UFUNCTION()
	void AddToKilLFeed(const FString& HitPlayerName, const FString& ShootingPlayerName);

	UFUNCTION()
	void SendMessageButtonOnPressed();

	UFUNCTION()
	void SendMessageToBox(FCustomChatMessage Message);

	UFUNCTION()
	void ClearChatBox();

	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};
