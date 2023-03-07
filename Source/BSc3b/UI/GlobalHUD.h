// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GlobalHUD.generated.h"

class UHorizontalBox;
class UScoreboardData;
/**
 * 
 */
//Forward Declarations
class UTextBlock;
class UGridPanel;
class UGridSlot;
class UKillFeedSlot;
class UVerticalBox;
class UMultiLineEditableTextBox;
class UChatBox;
class UBorder;
class AMenuGameState;

//Chat Message Struct
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
	FText Message;  //Message sent from multi-line text box

	UPROPERTY()
	FString TimeOfMessage;  //Takes in the hour and minute from the client local machine

	UPROPERTY()
	FString PlayerID;  //Platform ID
};

UCLASS()
class BSC3B_API UGlobalHUD : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

	/* Separate widget containing look and logic of killfeed*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess))
	TSubclassOf<UKillFeedSlot> KillFeedWidgetClass;

	UPROPERTY()
	UKillFeedSlot* KillFeedWidget;

	/* Separate widget containing what is seen by client when someone sends a message */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess))
	TSubclassOf<UChatBox> ChatBoxWidgetClass;
	
	UPROPERTY()
	UChatBox* ChatBoxWidget;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UBorder* ChatBoxUI;

	/* Separate widget containing what is seen by client when someone sends a message */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess))
	TSubclassOf<UScoreboardData> ScoreboardWidgetClass;
	
	UPROPERTY()
	UScoreboardData* ScoreboardWidget;


	/* Connect message that gets displayed upon new client joining session */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UTextBlock* MessageTextBox;

	//// CHAT WIDGET POINTERS ////
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UVerticalBox* AllChannelMessages;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UMultiLineEditableTextBox* MessageToSend;
	
	/* Container to spawn in kill feed class */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UGridPanel* NewKilLFeedBox;

	UPROPERTY()
	TArray<UGridSlot*> TotalKillFeedSlots;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UVerticalBox* Scoreboard;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess))
	UHorizontalBox* ScoreboardHeader;

	UFUNCTION()
	void ReachedMaximumKillFeedSlots();

	//Timer Handle variables
	UPROPERTY()
	FTimerHandle ConnectMessageHandle;

	UPROPERTY()
	FTimerHandle ChatMessageHandle;

	UFUNCTION()
	void RemoveSlotFromKillFeed(UKillFeedSlot* IncomingKillFeedSlot, UGridSlot* IncomingSlot);
	
	UFUNCTION()
	void HideChatBox();

	UPROPERTY()
	bool bIsCurrentlyTyping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	USoundBase* MessageRecieveSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	USoundBase* ChatTypingSound;


public:
	
	UFUNCTION()
	void SetConnectMessage(FText IncomingMessage);

	UFUNCTION()
	void SetConnectMessageVisibility(bool bShouldBeVisible);

	/* Focus the text box as opposed to clicking it with the mouse */
	UFUNCTION()
	void SetFocusToTextBox(APlayerController* PlayerController);

	/* Set kill feed variables and add it to KillFeedBox */
	UFUNCTION()
	void AddToKilLFeed(const FString& HitPlayerName, const FString& ShootingPlayerName);

	/* Called on Enter key and converts parameters to an FChatMessage and sending it to all current
	 * clients through a NetMulticast
	 */
	UFUNCTION()
	void SendMessageButtonOnPressed();

	/* Sends FChatMessage struct to the total channel messages vertical box */
	UFUNCTION()
	void SendMessageToBox(FCustomChatMessage Message);

	UFUNCTION()
	void ClearChatBox();

	UFUNCTION()
	void ShowScoreboard(bool bVisible, AMenuGameState* GS);

	static void SetTimerWithDelegate(FTimerHandle& TimerHandle, TBaseDelegate<void> ObjectDelegate, float Time,
		bool bLoop, UObject* WorldContextObject);

	/* Override Enter key while we are inside the multi-line text box otherwise it would
	 * go to next line 
	 */
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};
