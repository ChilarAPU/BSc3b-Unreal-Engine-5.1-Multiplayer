// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BSc3bController.generated.h"

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
public:
	
	ABSc3bController();

	virtual void BeginPlay() override;
	
};
