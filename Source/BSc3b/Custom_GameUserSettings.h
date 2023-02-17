// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "Custom_GameUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class BSC3B_API UCustom_GameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

	UCustom_GameUserSettings();

protected:
	UPROPERTY(Config)
	float PlayerSensitivity;

public:
	UFUNCTION(BlueprintCallable)
	void SetPlayerSensitivity(float Value);

	UFUNCTION(BlueprintPure)
	float GetPlayerSensitivity() const;

	/* Replace the original GetGameUserSettings function call with our own similar function which
	 * returns this class instead
	 */
	UFUNCTION(BlueprintCallable)
	static UCustom_GameUserSettings* GetCustomGameUserSettings();
};
