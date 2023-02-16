// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BSc3bGameMode.generated.h"

UCLASS(minimalapi)
class ABSc3bGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABSc3bGameMode();
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
};



