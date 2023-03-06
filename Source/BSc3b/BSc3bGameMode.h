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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	int TargetNumberOfKills;

public:
	ABSc3bGameMode();

	UFUNCTION()
	bool FlipSpawnPoint();

	UFUNCTION()
	int GetTargetNumberOfKills();

	/* */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	void PreLogout(APlayerController* InPlayerController);
	
	FUniqueNetIdPtr GetPlayerUniqueNetId(APlayerController* PlayerController);
};



