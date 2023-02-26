// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "EOS_GameInstance.generated.h"

/**
 * 
 */

UCLASS()
class BSC3B_API UEOS_GameInstance : public UGameInstance
{
	GENERATED_BODY()

	/* Holds the Epic ID of the owning client. Gets accessed by a multitude of systems*/
	UPROPERTY()
	FString PlayerName;

	/* Used in the main menu to give basic feedback for when the player makes server requests*/
	UPROPERTY()
	FString UserFeedback;

	/* The main session map to load. Has to be hard coded in C++ otherwise it does not work */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EOS Variables", meta = (AllowPrivateAccess))
	FString OpenLevelText;

	/* Holds a search of the current available sessions a client can join. Cannot be set as UPROPERTY*/
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	////// ON COMPLETE DELEGATES //////
	/* User Feedback on whether the login attempt was successful*/
	void LoginWithEOS_Return(int32 LocalUserNum, bool bWasSuccess, const FUniqueNetId& UserID, const FString& Error);
	/* if a session was successfully completed, Use ServerTravel to load the correct map*/
	void OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful);
	/* Currently bugged inside EOS, unused */
	void OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful);
	/* If we have found available sessions inside EOS, Join the first available session */
	void OnFindSessionCompleted(bool bWasSuccess);
	/* If the given session from above is available, use ClientTravel to load the correct map*/
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	/* TODO: Decrypt file that has been received from EOS cloud */
	void OnReadFileComplete(bool bSuccess, const FString& FileName);
	
public:

	//// CURRENTLY EXPOSED TO BLUEPRINT FOR EASE OF USE ////
	/* ID refers to the account email. Token refers to the account password. LoginType refers to the
	 * service to which we are trying to find the account from
	 */
	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void LoginWithEOS(FString ID, FString Token, FString LoginType);

	/* Get Player Username which gets the Epic ID and sets PlayerName */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EOS Functions")
	FString GetPlayerUsername();

	/* Check that player to logged in which is used mainly for adjusting the visibility of menu widgets*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EOS Functions")
	bool isPlayerLoggedIn();

	/* Create a session through the EOS portal with preset settings */
	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void CreateEOSSession(bool bIsDedicated, bool bIsLanServer, int32 NumberOfPublicConnections);

	/* Return a search of current available session to a max of 20 results*/
	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void FindSessionAndJoin();

	/* Called by EOS delegate to destroy the session. Currently bugged and does not work correctly*/
	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void DestroySession();

	/* Look for a specific file inside of the epic title cloud storage */
	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void GetTitleStorageInterface();

	/* Returns player back to the original/default map of the project */
	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	virtual void ReturnToMainMenu() override;
	
	UFUNCTION(BlueprintPure)
	FString GetMultiplayerFeedbackText();

	UFUNCTION()
	FString GetPlayerEpicID();

	
};
