// Fill out your copyright notice in the Description page of Project Settings.


#include "EOS_GameInstance.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"

void UEOS_GameInstance::LoginWithEOS(FString ID, FString Token, FString LoginType)
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (SubsystemRef)
	{
		IOnlineIdentityPtr IdentityPointerRef = SubsystemRef->GetIdentityInterface();
		if (IdentityPointerRef)
		{
			// Adding the account details into a struct that gets passed into Login()
			FOnlineAccountCredentials AccountDetails;
			AccountDetails.Id = ID;
			AccountDetails.Token = Token;
			AccountDetails.Type = LoginType;
			IdentityPointerRef->OnLoginCompleteDelegates->AddUObject(this, &UEOS_GameInstance::LoginWithEOS_Return);
			IdentityPointerRef->Login(0, AccountDetails);
		}
	}
	
}

FString UEOS_GameInstance::GetPlayerUsername()
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (SubsystemRef)
	{
		IOnlineIdentityPtr IdentityPointerRef = SubsystemRef->GetIdentityInterface();
		if (IdentityPointerRef)
		{
			//If user is logged in
			if (IdentityPointerRef->GetLoginStatus(0) == ELoginStatus::LoggedIn)
			{
				return IdentityPointerRef->GetPlayerNickname(0);
			}
		}
	}
	//If player is not logged in, return empty string
	return FString();
}

bool UEOS_GameInstance::isPlayerLoggedIn()
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (SubsystemRef)
	{
		IOnlineIdentityPtr IdentityPointerRef = SubsystemRef->GetIdentityInterface();
		if (IdentityPointerRef)
		{
			if (IdentityPointerRef)
			{
				//If user is logged in
				if (IdentityPointerRef->GetLoginStatus(0) == ELoginStatus::LoggedIn)
				{
					return true;
				}
			}
		}
	}
	return false;
	
}

void UEOS_GameInstance::LoginWithEOS_Return(int32 LocalUserNum, bool bWasSuccess, const FUniqueNetId& UserID,
                                            const FString& Error)
{
	if (bWasSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Login Success"));
		IOnlineSubsystem*  Subsystem = Online::GetSubsystem(this->GetWorld());
		IOnlineExternalUIPtr ExternalUI = Subsystem->GetExternalUIInterface();
		ExternalUI->ShowFriendsUI(0);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Login Failed: %s"), *Error);
		UserFeedback = TEXT("Login Failed, Please See Output Log for Further Details");
	}
}

void UEOS_GameInstance::OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UserFeedback = TEXT("Loading into Server...");
		GetWorld()->ServerTravel(FString("/Game/ThirdPerson/Maps/MainSessionMap?listen"));
		ServerName = SessionName;
	}
}

void UEOS_GameInstance::OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful)
{
	
}

void UEOS_GameInstance::OnFindSessionCompleted(bool bWasSuccess)
{
	if (bWasSuccess)
	{
		IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
		if (SubsystemRef)
		{
			IOnlineSessionPtr SessionPtrRef = SubsystemRef->GetSessionInterface();
			if (SessionPtrRef)
			{
				//Join the first available session
				if (SessionSearch->SearchResults.Num()>0)
				{
					SessionPtrRef->ClearOnJoinSessionCompleteDelegates(this);
					SessionPtrRef->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOS_GameInstance::OnJoinSessionCompleted);
					SessionPtrRef->JoinSession(0, FName("Main Session"), SessionSearch->SearchResults[0]);
				}
			}
		}
	}
	//If we have got here, it means that the player was not logged in
	UserFeedback = TEXT("Please Login Before trying to Find A Session");
	
}

void UEOS_GameInstance::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		if (APlayerController* PlayerControllerRef = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FString JoinAddress;
			IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
			if (SubsystemRef)
			{
				IOnlineSessionPtr SessionPtrRef = SubsystemRef->GetSessionInterface();
				if (SessionPtrRef)
				{
					SessionPtrRef->GetResolvedConnectString(FName("Main Session"), JoinAddress);
					UE_LOG(LogTemp, Warning, TEXT("Join Address: %s"), *JoinAddress);
					if (!JoinAddress.IsEmpty())
					{
						PlayerControllerRef->ClientTravel(JoinAddress, TRAVEL_Absolute);
					}
				}
			}
		}
	}
}

void UEOS_GameInstance::CreateEOSSession(bool bIsDedicated, bool bIsLanServer, int32 NumberOfPublicConnections)
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (SubsystemRef)
	{
		IOnlineSessionPtr SessionPtrRef = SubsystemRef->GetSessionInterface();
		if (SessionPtrRef)
		{
			// Set up the settings of the server
			FOnlineSessionSettings SessionCreationInfo;
			SessionCreationInfo.bIsDedicated = bIsDedicated;
			SessionCreationInfo.bAllowInvites = true;
			SessionCreationInfo.bIsLANMatch = bIsLanServer;
			SessionCreationInfo.NumPublicConnections = NumberOfPublicConnections;
			SessionCreationInfo.bUseLobbiesIfAvailable = false;
			SessionCreationInfo.bUsesPresence = false;
			SessionCreationInfo.bShouldAdvertise = true;
			SessionCreationInfo.bAllowJoinInProgress = true;
			
			SessionCreationInfo.Set(SEARCH_KEYWORDS, FString("RandomHi"), EOnlineDataAdvertisementType::ViaOnlineService);
			SessionPtrRef->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOS_GameInstance::OnCreateSessionCompleted);
			SessionPtrRef->CreateSession(0, FName("Main Session"), SessionCreationInfo);
			SessionPtrRef->StartSession(FName("Main Session"));
		}
	}
}

void UEOS_GameInstance::FindSessionAndJoin()
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (SubsystemRef)
	{
		IOnlineSessionPtr SessionPtrRef = SubsystemRef->GetSessionInterface();
		if (SessionPtrRef)
		{
			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			SessionSearch->bIsLanQuery = false;
			SessionSearch->MaxSearchResults = 20;
			SessionSearch->QuerySettings.SearchParams.Empty();
			SessionPtrRef->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOS_GameInstance::OnFindSessionCompleted);
			SessionPtrRef->FindSessions(0, SessionSearch.ToSharedRef());
		}
	}
}

void UEOS_GameInstance::JoinSession()
{
	
}

void UEOS_GameInstance::DestroySession()
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	UE_LOG(LogTemp, Warning, TEXT("Session Destroyed 1"));
	if (SubsystemRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Destroyed 2"));
		IOnlineSessionPtr SessionPtrRef = SubsystemRef->GetSessionInterface();
		if (SessionPtrRef)
		{
			SessionPtrRef->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOS_GameInstance::OnDestroySessionCompleted);
			SessionPtrRef->DestroySession(FName("Main Session"));
			UE_LOG(LogTemp, Warning, TEXT("Session Destroyed 3"));
		}
	}
	
}

void UEOS_GameInstance::ReturnToMainMenu()
{
	Super::ReturnToMainMenu();
}
