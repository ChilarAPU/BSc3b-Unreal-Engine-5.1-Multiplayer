// Copyright Epic Games, Inc. All Rights Reserved.

#include "BSc3bGameMode.h"
#include "BSc3bCharacter.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "UObject/ConstructorHelpers.h"

ABSc3bGameMode::ABSc3bGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_PlayerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ABSc3bGameMode::PostLogin(APlayerController* NewPlayer)
{

	Super::PostLogin(NewPlayer);
	if (NewPlayer)
	{
		FUniqueNetIdRepl UniqueNetIDRepl;
		//Checking that we are the controller that hosts the session
		if (NewPlayer->IsLocalController())
		{
			ULocalPlayer* LocalPlayerRef = NewPlayer->GetLocalPlayer();
			if (LocalPlayerRef)
			{
				UniqueNetIDRepl = LocalPlayerRef->GetPreferredUniqueNetId();
			}
			else
			{
				UNetConnection* RemoteNetConnectionRef = Cast<UNetConnection>(NewPlayer->Player);
				check(IsValid(RemoteNetConnectionRef));
				UniqueNetIDRepl = RemoteNetConnectionRef->PlayerId;
			}
		}
		else //Client trying to join the session
		{
			UNetConnection* RemoteNetConnectionRef = Cast<UNetConnection>(NewPlayer->Player);
			check(IsValid(RemoteNetConnectionRef));
			UniqueNetIDRepl = RemoteNetConnectionRef->PlayerId;
		}

		TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIDRepl.GetUniqueNetId();
		if (UniqueNetId)
		{
			IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(NewPlayer->GetWorld());
			IOnlineSessionPtr SessionRef = SubsystemRef->GetSessionInterface();
			bool bRegistrationSuccess = SessionRef->RegisterPlayer(FName("Main Session"), *UniqueNetId, false);
			if (bRegistrationSuccess)
			{
				UE_LOG(LogTemp, Warning, TEXT("Registration Successful"));
			}
		}
		
	}
}
