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
	IOnlineIdentityPtr IdentityPointerRef = GetIdentityInterface();
	// Adding the account details into a struct that gets passed into Login()
	FOnlineAccountCredentials AccountDetails;
	AccountDetails.Id = ID;
	AccountDetails.Token = Token;
	AccountDetails.Type = LoginType;
	IdentityPointerRef->OnLoginCompleteDelegates->AddUObject(this, &UEOS_GameInstance::LoginWithEOS_Return);
	IdentityPointerRef->Login(0, AccountDetails);
	SendUserFeedback(TEXT("Logging in..."));
}

FString UEOS_GameInstance::GetPlayerUsername()
{
	IOnlineIdentityPtr IdentityPointerRef = GetIdentityInterface();
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
	IOnlineIdentityPtr IdentityPointerRef = GetIdentityInterface();
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
	IOnlineIdentityPtr IdentityPointerRef = GetIdentityInterface();
	if (bWasSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Login Success"));
		SendUserFeedback(TEXT("Successfully Logged into Epic"));

		//Automatically show friends overlay upon logging in
		IOnlineSubsystem*  Subsystem = Online::GetSubsystem(this->GetWorld());
		IOnlineExternalUIPtr ExternalUI = Subsystem->GetExternalUIInterface();
		ExternalUI->ShowFriendsUI(0); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Login Failed: %s"), *Error);
		SendUserFeedback(TEXT("Login Failed, Please See Output Log for Further Details"));
	}
	IdentityPointerRef->ClearOnLoginCompleteDelegates(LocalUserNum, this);
}

void UEOS_GameInstance::OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		SendUserFeedback(TEXT("Loading into Server..."), true);
		GetWorld()->ServerTravel(FString("/Game/ThirdPerson/Maps/MainSessionMap?listen"));
	}
	IOnlineSessionPtr SessionPtrRef = GetSessionInterface();
	SessionPtrRef->ClearOnCreateSessionCompleteDelegates(this);
}

void UEOS_GameInstance::OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful)
{
	//Currently not working due to bug in EOS
	IOnlineSessionPtr SessionPtrRef = GetSessionInterface();
	SessionPtrRef->ClearOnDestroySessionCompleteDelegates(this);
}

void UEOS_GameInstance::OnFindSessionCompleted(bool bWasSuccess)
{
	if (!bWasSuccess)
	{
		return;
	}
	
	IOnlineSessionPtr SessionPtrRef = GetSessionInterface();

	//Access pawn so we can indirectly access widget
	AMenuPawn* PlayerRef = Cast<AMenuPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!PlayerRef)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Display Session Delegate"));
	PlayerRef->MainMenu->ShowServerBrowser(); //Show server browser widget
	
	for (int i = 0; i < SessionSearch->SearchResults.Num(); i++) //Loop through each available session
	{
		//for each session, add a new child to the server browser container
		if (ServerDataSlotClass)
		{
			ServerDataSlot = CreateWidget<UServerSlot>(GetWorld(), ServerDataSlotClass);
			//Send over data for the server slot that will be visible on browser
			ServerDataSlot->SetServerPing(SessionSearch->SearchResults[i].PingInMs);
			ServerDataSlot->SetServerName(SessionSearch->SearchResults[i].Session.OwningUserName);
			ServerDataSlot->SetTotalCurrentPlayersInServer(SessionSearch->SearchResults[i].Session.NumOpenPrivateConnections,
				SessionSearch->SearchResults[i].Session.SessionSettings.NumPublicConnections);
			ServerDataSlot->PlaceInSearchResult = i; //Used with joining a session
			PlayerRef->MainMenu->AddSlotToServerBrowser(ServerDataSlot);
		}
	}
	SessionPtrRef->ClearOnFindSessionsCompleteDelegates(this);
	SendUserFeedback(TEXT("Available Sessions Found"));
	
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
		
	IOnlineSessionPtr SessionPtrRef = GetSessionInterface();

	SessionPtrRef->ClearOnJoinSessionCompleteDelegates(this);

	// Join the selected session server and world once the server has registered the client
	SessionPtrRef->GetResolvedConnectString(ServerPassword, JoinAddress); //Get the address of the server
	UE_LOG(LogTemp, Warning, TEXT("Join Address: %s"), *JoinAddress);  //Debug purposes
	PlayerControllerRef->ClientTravel(JoinAddress, TRAVEL_Absolute);  //Client join server map

	SendUserFeedback(TEXT("Request Accepted. Loading Map..."), true);
	
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

void UEOS_GameInstance::OnCreateNewSession(bool bWasSuccess)
{
	if (!bWasSuccess)
	{
		return;
	}
	IOnlineSessionPtr SessionPtrRef = GetSessionInterface();

	SessionPtrRef->ClearOnFindSessionsCompleteDelegates(this);

	FOnlineSessionSettings SessionCreationInfo;
	SessionCreationInfo.bIsDedicated = false;
	SessionCreationInfo.bAllowInvites = true;
	SessionCreationInfo.bIsLANMatch = false;
	SessionCreationInfo.NumPublicConnections = 10;
	SessionCreationInfo.bUseLobbiesIfAvailable = false;
	SessionCreationInfo.bUsesPresence = false;
	SessionCreationInfo.bShouldAdvertise = true;

	// Get the password from the editable text box on the menu
	AMenuPawn* PlayerRef = Cast<AMenuPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!PlayerRef)
	{
		return;
	}
	ServerPassword = FName(PlayerRef->MainMenu->GetServerPassword());

	//Set key so session can be searched through the dev portal
	SessionCreationInfo.Set(SEARCH_KEYWORDS, FString("RandomHi"), EOnlineDataAdvertisementType::ViaOnlineService);
	//Join session and map for the user once server has been created
	SessionPtrRef->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOS_GameInstance::OnCreateSessionCompleted);
	//Create session with the given name
	bool bCreationSuccessful = SessionPtrRef->CreateSession(0, ServerPassword, SessionCreationInfo);
	SendUserFeedback(TEXT("Empty Server Found"));
	
	if (!bCreationSuccessful) //If we could not create a new session due to naming conflict
	{
		SendUserFeedback(TEXT("Server With That Name Exists. Please Choose Another"));
	}
	
}

IOnlineSessionPtr UEOS_GameInstance::GetSessionInterface()
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (!SubsystemRef)
	{
		return nullptr;
	}
	IOnlineSessionPtr SessionPtrRef = SubsystemRef->GetSessionInterface();
	if (!SessionPtrRef)
	{
		return nullptr;
	}
	return SessionPtrRef;
}

IOnlineIdentityPtr UEOS_GameInstance::GetIdentityInterface()
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (!SubsystemRef)
	{
		return nullptr;
	}
	IOnlineIdentityPtr IdentityPointerRef = SubsystemRef->GetIdentityInterface();
	if (!IdentityPointerRef)
	{
		return nullptr;
	}
	return IdentityPointerRef;
}

void UEOS_GameInstance::SendUserFeedback(FString FeedbackToShow, bool bShouldNotDisappear)
{
	UserFeedback = FeedbackToShow;
	
	AMenuPawn* PlayerRef = Cast<AMenuPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!PlayerRef)
	{
		return;
	}
	PlayerRef->MainMenu->ShowUserFeedback(bShouldNotDisappear);
}

void UEOS_GameInstance::CreateEOSSession(bool bIsDedicated, bool bIsLanServer, int32 NumberOfPublicConnections)
{
	IOnlineSessionPtr SessionPtrRef = GetSessionInterface();
	
	// preset settings of the server
	//Preset settings for client searching for sessions
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 20;
	SessionSearch->QuerySettings.SearchParams.Empty();  //If we want to only search for a specific set of sessions
	
	SessionPtrRef->FindSessions(0, SessionSearch.ToSharedRef());
	SessionPtrRef->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOS_GameInstance::OnCreateNewSession);

	SendUserFeedback(TEXT("Locating Empty Server Slot..."));
	
}

void UEOS_GameInstance::FindSessionsAndDisplayBrowser()
{
	IOnlineSessionPtr SessionPtrRef = GetSessionInterface();
	
	//Preset settings for client searching for sessions
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 20;
	SessionSearch->QuerySettings.SearchParams.Empty();  //If we want to only search for a specific set of sessions
	UE_LOG(LogTemp, Warning, TEXT("Display Sessions"));

	AMenuPawn* PlayerRef = Cast<AMenuPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!PlayerRef)
	{
		return;
	}

	PlayerRef->MainMenu->ClearServerBrowser(); //Clear the browser before adding in any new slots

	//Upon finding all available sessions, go to OnFindSessionCompleted which will display all sessions in a browser
	SessionPtrRef->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOS_GameInstance::OnFindSessionCompleted);
	SessionPtrRef->FindSessions(0, SessionSearch.ToSharedRef());

	SendUserFeedback(TEXT("Finding Available Sessions..."));
}

void UEOS_GameInstance::DestroySession()
{
	IOnlineSessionPtr SessionPtrRef = GetSessionInterface();
	
	SessionPtrRef->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOS_GameInstance::OnDestroySessionCompleted);
	SessionPtrRef->DestroySession(ServerPassword);
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
	
	IOnlineSessionPtr SessionPtrRef = GetSessionInterface();

	AMenuPawn* PlayerRef = Cast<AMenuPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!PlayerRef)
	{
		return;
	}
	ServerPassword = FName(PlayerRef->MainMenu->GetServerPassword());
	
	SessionPtrRef->JoinSession(0, ServerPassword, SessionSearch->SearchResults[LocationInBrowser]);
	SessionPtrRef->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOS_GameInstance::OnJoinSessionCompleted);

	SendUserFeedback(TEXT("Attempting To Joining Selected Session..."));
}

void UEOS_GameInstance::ReturnToMainMenu()
{
	SendUserFeedback(TEXT("")); //Empty user feedback upon returning to the main menu
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
