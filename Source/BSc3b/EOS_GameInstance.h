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

public:
	
	/* ID refers to the account email. Token refers to the account password. LoginType refers to the
	 * service to which we are trying to find the account from
	 */
	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void LoginWithEOS(FString ID, FString Token, FString LoginType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EOS Functions")
	FString GetPlayerUsername();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EOS Functions")
	bool isPlayerLoggedIn();

	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void CreateEOSSession(bool bIsDedicated, bool bIsLanServer, int32 NumberOfPublicConnections);

	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void FindSessionAndJoin();

	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void JoinSession();

	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void DestroySession();

	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	virtual void ReturnToMainMenu() override;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EOS Variables")
	FString OpenLevelText;

	UPROPERTY(BlueprintReadOnly)
	FName ServerName;

	UPROPERTY(BlueprintReadOnly)
	FString UserFeedback;

	////// ON COMPLETE DELGATES //////
	void LoginWithEOS_Return(int32 LocalUserNum, bool bWasSuccess, const FUniqueNetId& UserID, const FString& Error);
	void OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful);
	void OnFindSessionCompleted(bool bWasSuccess);
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	
};
