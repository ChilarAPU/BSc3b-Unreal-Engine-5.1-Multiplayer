// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MenuGameState.generated.h"

/**
 * 
 */
UCLASS()
class BSC3B_API AMenuGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Replicated)
	FName ServerName;

	UFUNCTION()
	void ChangeServerName();
	
	/* All replicated values must be called in this function with any conditions */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
