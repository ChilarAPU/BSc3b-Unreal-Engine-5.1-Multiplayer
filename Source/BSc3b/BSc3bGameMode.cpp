// Copyright Epic Games, Inc. All Rights Reserved.

#include "BSc3bGameMode.h"

#include "EOS_GameInstance.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ABSc3bGameMode::ABSc3bGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_PlayerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	bSpawnPoint = false;
}

bool ABSc3bGameMode::FlipSpawnPoint()
{
	if (bSpawnPoint)
	{
		bSpawnPoint = false;
	}
	else
	{
		bSpawnPoint = true;
	}
	return bSpawnPoint;
}

void ABSc3bGameMode::PostLogin(APlayerController* NewPlayer)
{

	Super::PostLogin(NewPlayer);
		TSharedPtr<const FUniqueNetId> UniqueNetId = GetPlayerUniqueNetId(NewPlayer);
		if (UniqueNetId)
		{
			IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(NewPlayer->GetWorld());
			IOnlineSessionPtr SessionRef = SubsystemRef->GetSessionInterface();
			UEOS_GameInstance* GI = Cast<UEOS_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
			UE_LOG(LogTemp, Warning, TEXT("%s"), *GI->ServerPassword.ToString());
			bool bRegistrationSuccess = SessionRef->RegisterPlayer(FName(TEXT("MainSession")), *UniqueNetId, false);
			if (bRegistrationSuccess)
			{
				UE_LOG(LogTemp, Warning, TEXT("Registration Successful"));
			}
		}
}

void ABSc3bGameMode::PreLogout(APlayerController* InPlayerController)
{
		TSharedPtr<const FUniqueNetId> UniqueNetId = GetPlayerUniqueNetId(InPlayerController);
		if (UniqueNetId)
		{
			IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(InPlayerController->GetWorld());
			IOnlineSessionPtr SessionRef = SubsystemRef->GetSessionInterface();
			UEOS_GameInstance* GI = Cast<UEOS_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
			UE_LOG(LogTemp, Warning, TEXT("%s"), *GI->ServerPassword.ToString());
			bool bRegistrationSuccess = SessionRef->UnregisterPlayer(FName(TEXT("MainSession")), *UniqueNetId);
			if (bRegistrationSuccess)
			{
				UE_LOG(LogTemp, Warning, TEXT("Unregistration Successful"));
			}
		}
}

FUniqueNetIdPtr ABSc3bGameMode::GetPlayerUniqueNetId(APlayerController* PlayerController)
{
	if (!IsValid(PlayerController))
	{
		return nullptr;
	}
	if (PlayerController)
	{
		FUniqueNetIdRepl UniqueNetIDRepl;
		//Checking that we are the controller that hosts the session
		if (PlayerController->IsLocalController())
		{
			ULocalPlayer* LocalPlayerRef = PlayerController->GetLocalPlayer();
			if (LocalPlayerRef)
			{
				UniqueNetIDRepl = LocalPlayerRef->GetPreferredUniqueNetId();
			}
			else
			{
				UNetConnection* RemoteNetConnectionRef = Cast<UNetConnection>(PlayerController->Player);
				check(IsValid(RemoteNetConnectionRef));
				UniqueNetIDRepl = RemoteNetConnectionRef->PlayerId;
			}
		}
		else //Client trying to join the session
			{
			UNetConnection* RemoteNetConnectionRef = Cast<UNetConnection>(PlayerController->Player);
			check(IsValid(RemoteNetConnectionRef));
			UniqueNetIDRepl = RemoteNetConnectionRef->PlayerId;
			}
		return UniqueNetIDRepl.GetUniqueNetId();
	}
	//If the player controller is invalid, return NULL
	return nullptr;
}
