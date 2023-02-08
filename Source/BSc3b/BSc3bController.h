// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BSc3bController.generated.h"

class UPlayerAnimation;
class UPlayerHUD;
class ABSc3bCharacter;
/**
 * 
 */

UCLASS()
class BSC3B_API ABSc3bController : public APlayerController
{
	GENERATED_BODY()
	
	UPROPERTY()
	ABSc3bCharacter* PlayerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UPlayerAnimation> PlayerAnimClass;


public:
	
	ABSc3bController();

	virtual void BeginPlay() override;
	
	UPROPERTY()
	UPlayerHUD* PlayerHUD;

	/* Encapsulate the TSubclassOf variable into an easier to access and read value */
	UPROPERTY()
	UPlayerAnimation* PlayerAnim;

	UFUNCTION()
	void ShowRespawnButton(bool Visible);
	
};
