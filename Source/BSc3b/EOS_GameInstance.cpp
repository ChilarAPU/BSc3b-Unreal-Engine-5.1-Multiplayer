// Fill out your copyright notice in the Description page of Project Settings.


#include "EOS_GameInstance.h"

#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MenuPawn.h"
#include "UI/MainMenu.h"
#include "UI/ServerSlot.h"

void UEOS_GameInstance::LoginWithEOS(FString ID, FString Token, FString LoginType)
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (!SubsystemRef)
	{
		return;
	}
	IOnlineIdentityPtr IdentityPointerRef = SubsystemRef->GetIdentityInterface();
	if (!IdentityPointerRef)
	{
		return;
	}
	// Adding the account details into a struct that gets passed into Login()
	FOnlineAccountCredentials AccountDetails;
	AccountDetails.Id = ID;
	AccountDetails.Token = Token;
	AccountDetails.Type = LoginType;
	IdentityPointerRef->OnLoginCompleteDelegates->AddUObject(this, &UEOS_GameInstance::LoginWithEOS_Return);
	IdentityPointerRef->Login(0, AccountDetails);
	
}

FString UEOS_GameInstance::GetPlayerUsername()
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (!SubsystemRef)
	{
		return FString();
	}
	
	IOnlineIdentityPtr IdentityPointerRef = SubsystemRef->GetIdentityInterface();
	if (!IdentityPointerRef)
	{
		return FString();
	}
	
	//If user is logged in
	if (IdentityPointerRef->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		PlayerName = IdentityPointerRef->GetPlayerNickname(0);
		return IdentityPointerRef->GetPlayerNickname(0);
	}
	
	//If player is not logged in, return empty string
	return FString();
}

bool UEOS_GameInstance::isPlayerLoggedIn()
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (!SubsystemRef)
	{
		return false;
	}
	IOnlineIdentityPtr IdentityPointerRef = SubsystemRef->GetIdentityInterface();
	if (!IdentityPointerRef)
	{
		return false;
	}
	//If user is logged in
	if (IdentityPointerRef->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		return true;
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
		ExternalUI->ShowFriendsUI(0);  //Automatically show friends overlay upon logging in
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
	}
}

void UEOS_GameInstance::OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful)
{
	//Currently not working due to bug in EOS
}

void UEOS_GameInstance::OnFindSessionCompleted(bool bWasSuccess)
{
	if (!bWasSuccess)
	{
		return;
	}
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (!SubsystemRef)
	{
		return;
	}
	IOnlineSessionPtr SessionPtrRef = SubsystemRef->GetSessionInterface();
	if (!SessionPtrRef)
	{
		return;
	}
	
	//Join the first available session
	if (SessionSearch->SearchResults.Num()>0)
	{
		SessionPtrRef->ClearOnJoinSessionCompleteDelegates(this);  //Not sure if this is needed
		SessionPtrRef->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOS_GameInstance::OnJoinSessionCompleted);
		//Join first session inside the session interface inside the EOS, does not effect client program
		
		SessionPtrRef->JoinSession(0, FName(TEXT("MainSession")), SessionSearch->SearchResults[0]);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *SessionSearch->SearchResults[0].GetSessionIdStr());
		return;
	}
	
	//If we have got here, it means that the player was not logged in
	UserFeedback = TEXT("Please Login Before trying to Find A Session");
	
}

void UEOS_GameInstance::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		return;
	}
	APlayerController* PlayerControllerRef = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerControllerRef)
	{
		return;
	}
	FString JoinAddress;  //Will hold the address of our server
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (!SubsystemRef)
	{
		return;
	}
	IOnlineSessionPtr SessionPtrRef = SubsystemRef->GetSessionInterface();
	if (!SessionPtrRef)
	{
		return;
	}
	
	AMenuPawn* PlayerRef = Cast<AMenuPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!PlayerRef)
	{
		return;
	}
	PlayerRef->MainMenu->ShowServerBrowser();
	for (int i = 0; i < SessionSearch->SearchResults.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("WW"));
		if (ServerDataSlotClass)
		{
			ServerDataSlot = CreateWidget<UServerSlot>(GetWorld(), ServerDataSlotClass);
			ServerDataSlot->SetServerName(SessionSearch->SearchResults[i].Session.OwningUserName);
			ServerDataSlot->PlaceInSearchResult = i;
			PlayerRef->MainMenu->AddSlotToServerBrowser(ServerDataSlot);
		}
	}
	
}

void UEOS_GameInstance::OnReadFileComplete(bool bSuccess, const FString& FileName)
{
	if(!bSuccess)
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("File Read"));
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (!SubsystemRef)
	{
		return;
	}
	IOnlineTitleFilePtr TitleFileRef = SubsystemRef->GetTitleFileInterface();
	if (!TitleFileRef)
	{
	}
}

void UEOS_GameInstance::CreateEOSSession(bool bIsDedicated, bool bIsLanServer, int32 NumberOfPublicConnections)
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (!SubsystemRef)
	{
		return;
	}
	IOnlineSessionPtr SessionPtrRef = SubsystemRef->GetSessionInterface();
	if (!SessionPtrRef)
	{
		return;
	}
	// preset settings of the server
	//Preset settings for client searching for sessions
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 20;
	SessionSearch->QuerySettings.SearchParams.Empty();  //If we want to only search for a specific set of sessions
	
	/*SessionPtrRef->FindSessions(0, SessionSearch.ToSharedRef());
	SessionPtrRef->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOS_GameInstance::OnNewFindSessionCompleted);
	*/
	
	FOnlineSessionSettings SessionCreationInfo;
	SessionCreationInfo.bIsDedicated = bIsDedicated;
	SessionCreationInfo.bAllowInvites = true;
	SessionCreationInfo.bIsLANMatch = bIsLanServer;
	SessionCreationInfo.NumPublicConnections = NumberOfPublicConnections;
	SessionCreationInfo.bUseLobbiesIfAvailable = false;
	SessionCreationInfo.bUsesPresence = false;
	SessionCreationInfo.bShouldAdvertise = true;
	//SessionCreationInfo.bAllowJoinInProgress = true; For some reason stops players from being able to join

	//Set key so session can be searched through the dev portal
	SessionCreationInfo.Set(SEARCH_KEYWORDS, FString("RandomHi"), EOnlineDataAdvertisementType::ViaOnlineService);
	//Join session and map for the user once server has been created
	SessionPtrRef->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOS_GameInstance::OnCreateSessionCompleted);
	//create session on the server
	//NAME_GameSession
	AMenuPawn* PlayerRef = Cast<AMenuPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!PlayerRef)
	{
		return;
	}
	ServerPassword = FName(PlayerRef->MainMenu->GetServerPassword());
	SessionPtrRef->CreateSession(0, FName(TEXT("MainSession")), SessionCreationInfo);
	//SessionPtrRef->StartSession(FName("Main Session"));
	
	
}

void UEOS_GameInstance::FindSessionAndJoin()
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (!SubsystemRef)
	{
		return;
	}
	IOnlineSessionPtr SessionPtrRef = SubsystemRef->GetSessionInterface();
	if (!SessionPtrRef)
	{
		return;
	}
	//Preset settings for client searching for sessions
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 20;
	SessionSearch->QuerySettings.SearchParams.Empty();  //If we want to only search for a specific set of sessions
	//Join session upon successfully finding available sessions
	SessionPtrRef->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOS_GameInstance::OnFindSessionCompleted);
	SessionPtrRef->FindSessions(0, SessionSearch.ToSharedRef());
}

void UEOS_GameInstance::DestroySession()
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (!SubsystemRef)
	{
		return;
	}
	IOnlineSessionPtr SessionPtrRef = SubsystemRef->GetSessionInterface();
	if (!SessionPtrRef)
	{
		return;
	}
	SessionPtrRef->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOS_GameInstance::OnDestroySessionCompleted);
	SessionPtrRef->DestroySession(FName(TEXT("MainSession")));
}

void UEOS_GameInstance::GetTitleStorageInterface()
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (!SubsystemRef)
	{
		return;
	}
	IOnlineTitleFilePtr TitleFileRef = SubsystemRef->GetTitleFileInterface();
	if (!TitleFileRef)
	{
		return;
	}
	FString FileName = TEXT("Server_Names");  //Name of file on the cloud
	bool bContainsFile = TitleFileRef->ReadFile(FileName);  //Start reading the file
	TitleFileRef->OnReadFileCompleteDelegates.AddUObject(this, &UEOS_GameInstance::OnReadFileComplete);
	// return if the file does not exist on the cloud
	if (!bContainsFile)
	{
		UE_LOG(LogTemp, Warning, TEXT("File Name: %s Does Not Exist"), *FileName);
	}
}

void UEOS_GameInstance::JoinSelectedSession(int LocationInBrowser)
{
	APlayerController* PlayerControllerRef = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerControllerRef)
	{
		return;
	}
	FString JoinAddress;  //Will hold the address of our server
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (!SubsystemRef)
	{
		return;
	}
	IOnlineSessionPtr SessionPtrRef = SubsystemRef->GetSessionInterface();
	if (!SessionPtrRef)
	{
		return;
	}

	AMenuPawn* PlayerRef = Cast<AMenuPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!PlayerRef)
	{
		return;
	}
	ServerPassword = FName(PlayerRef->MainMenu->GetServerPassword());
	
	SessionPtrRef->GetResolvedConnectString(FName(TEXT("MainSession")), JoinAddress); //Get the address of the server
	UE_LOG(LogTemp, Warning, TEXT("Join Address: %s"), *JoinAddress);  //Debug purposes
	if (!JoinAddress.IsEmpty())  //Make sure we are not trying to join an empty server, would result in a crash
	{
		//SessionPtrRef->JoinSession(0, FName(TEXT("MainSession")), SessionSearch->SearchResults[LocationInBrowser]);
		PlayerControllerRef->ClientTravel(JoinAddress, TRAVEL_Absolute);  //Client join server map
	}
	
}

void UEOS_GameInstance::ReturnToMainMenu()
{
	Super::ReturnToMainMenu();
}

FString UEOS_GameInstance::GetPlayerEpicID()
{
	return PlayerName;
}

FString UEOS_GameInstance::GetMultiplayerFeedbackText()
{
	return UserFeedback;
}
