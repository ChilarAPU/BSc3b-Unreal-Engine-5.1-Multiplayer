// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MenuGameState.generated.h"

class UGlobalHUD;
/**
 * 
 */

UCLASS()
class BSC3B_API AMenuGameState : public AGameStateBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess))
	TSubclassOf<UGlobalHUD> ClientOnlyWidgetClass;

	/* Widget that is replicated to all clients screens at the same time */
	UPROPERTY()
	UGlobalHUD* ClientOnlyWidget;

public:
	
	UFUNCTION()
	UGlobalHUD* GetGlobalWidget();

	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_EndSession(const FString& WinningPlayerID);
	void Multicast_EndSession_Implementation(const FString& WinningPlayerID);

	UFUNCTION()
	void EndSession();
	
};
