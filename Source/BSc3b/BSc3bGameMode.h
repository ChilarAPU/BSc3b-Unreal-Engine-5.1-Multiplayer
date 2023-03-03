// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BSc3bGameMode.generated.h"

UCLASS(minimalapi)
class ABSc3bGameMode : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY()
	bool bSpawnPoint;

	UPROPERTY()
	FName CurrentServerName;

public:
	ABSc3bGameMode();

	UFUNCTION()
	bool FlipSpawnPoint();

	/* */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	void PreLogout(APlayerController* InPlayerController);
	
	FUniqueNetIdPtr GetPlayerUniqueNetId(APlayerController* PlayerController);
};



