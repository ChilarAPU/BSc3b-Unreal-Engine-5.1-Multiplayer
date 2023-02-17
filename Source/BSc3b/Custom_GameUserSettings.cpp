// Fill out your copyright notice in the Description page of Project Settings.


#include "Custom_GameUserSettings.h"

UCustom_GameUserSettings::UCustom_GameUserSettings()
{
	PlayerSensitivity = 1;
}

void UCustom_GameUserSettings::SetPlayerSensitivity(float Value)
{
	PlayerSensitivity = Value;
}

float UCustom_GameUserSettings::GetPlayerSensitivity() const
{
	return PlayerSensitivity;
}

UCustom_GameUserSettings* UCustom_GameUserSettings::GetCustomGameUserSettings()
{
	return Cast<UCustom_GameUserSettings>(UGameUserSettings::GetGameUserSettings());
}
