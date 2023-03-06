// Fill out your copyright notice in the Description page of Project Settings.


#include "BSc3bController.h"

#include "BSc3bCharacter.h"
#include "../BSc3bGameMode.h"
#include "../Custom_GameUserSettings.h"
#include "../EOS_GameInstance.h"
#include "../UI/InGameMenu.h"
#include "../UI/PlayerHUD.h"
#include "../Weapon/Weapon.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ABSc3bController::ABSc3bController()
{
	
}

void ABSc3bController::BeginPlay()
{
	Super::BeginPlay();
	//Cast to our character in case we ever need it
	PlayerClass = Cast<ABSc3bCharacter>(GetCharacter());

	//Set and spawn our HUD onto the client
	if (!IsValid(PlayerHUDClass))
	{
		return;
	}
	if (IsLocalPlayerController())
	{
		PlayerHUD = CreateWidget<UPlayerHUD>(GetWorld(), PlayerHUDClass);
		PlayerHUD->AddToViewport();	
	}

	//Get our custom Game settings class so we can access and set its values
	UserSettings = Cast<UCustom_GameUserSettings>(UserSettings->GetGameUserSettings());
	
	if (PlayerClass)
	{
		UEOS_GameInstance* GI = Cast<UEOS_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		PlayerClass->Server_PlaySpawnMessage(GI->GetPlayerEpicID());
	}
}

UPlayerHUD* ABSc3bController::GetPlayerHUD()
{
	return PlayerHUD;
}

void ABSc3bController::SpawnInGameMenu()
{
	if (!IsValid(InGameMenuClass))
	{
		return;
	}
	SetShowMouseCursor(true);
	SetIgnoreLookInput(true);
	InGameMenuWidget = CreateWidget<UInGameMenu>(GetWorld(), InGameMenuClass);
	InGameMenuWidget->AddToViewport();
	const FInputModeGameAndUI Input;
	SetInputMode(Input);
}

void ABSc3bController::RemoveInGameMenu()
{
	if (!IsValid(InGameMenuWidget))
	{
		return;
	}
	SetShowMouseCursor(false);
	SetIgnoreLookInput(false);
	InGameMenuWidget->RemoveFromParent();
	const FInputModeGameOnly Input;
	SetInputMode(Input);
}

void ABSc3bController::ShowRespawnButton(bool Visible)
{
	if (IsValid(PlayerHUD))
	{
		//Toggle mouse cursor and look input on controller
		SetShowMouseCursor(Visible);
		SetIgnoreLookInput(Visible);
		if (Visible)
		{
			PlayerHUD->RespawnButton->SetVisibility(ESlateVisibility::Visible);
			const FInputModeUIOnly Input;
			SetInputMode(Input);
		}
		else
		{
			PlayerHUD->RespawnButton->SetVisibility(ESlateVisibility::Hidden);
			const FInputModeGameOnly Input;
			SetInputMode(Input);
		}
		
	}
	
}

USoundBase* ABSc3bController::GetClothSound()
{
	return ClothSound;
}

USoundBase* ABSc3bController::GetGunshotSound()
{
	return Gunshot;
}

USoundBase* ABSc3bController::GetEmptyGunshotSound()
{
	return EmptyGunshot;
}

USoundBase* ABSc3bController::GetPlayerHitSound()
{
	return PlayerHit;
}

USoundBase* ABSc3bController::GetLaserSightOnSound()
{
	return LaserSightOn;
}

USoundBase* ABSc3bController::GetLaserSightOffSound()
{
	return LaserSightOff;
}

USoundAttenuation* ABSc3bController::GetGunshotAttenuation()
{
	return GunshotAttenuation;
}

TSubclassOf<ABullet> ABSc3bController::GetBulletClass()
{
	return SpawnObject;
}

UCustom_GameUserSettings* ABSc3bController::GetConfigUserSettings()
{
	return UserSettings;
}

void ABSc3bController::WeaponSway(float DeltaTime, FVector2D LookAxis, UWeapon* Weapon)
{
	if (!IsValid(Weapon))
	{
		return;
	}
	//Values the gun will be interpolating between
	FRotator FinalRot, InitRot;
	//Maximum value the gun can sway
	const float MaxSwayDegree = 2.5;
	//Speed at which the gun reaches the final rotation value
	const float InterpSpeed = 2.5;
	//Multiplying each lookaxis vector value by -1 flips the incoming direction
	//LookAxisVector relates to the speed of our mouse movement
	FinalRot = FRotator(LookAxis.Y * -1 * MaxSwayDegree, LookAxis.X * -1 * MaxSwayDegree, LookAxis.X * -1 * MaxSwayDegree);
	FRotator RotationDifference = UKismetMathLibrary::MakeRotator(InitRot.Roll + FinalRot.Roll, InitRot.Pitch - FinalRot.Pitch, InitRot.Yaw + FinalRot.Yaw);
	//Do all our interping in relative space as this keeps our values to normal amounts
	FRotator InterpDifference = UKismetMathLibrary::RInterpTo(Weapon->GetRelativeRotation(), RotationDifference, DeltaTime, InterpSpeed);
	//Variables to hold the break rotator function call
	float Roll, Pitch, Yaw;
	UKismetMathLibrary::BreakRotator(InterpDifference, Roll, Pitch, Yaw);
	//Clamp all values between MaxSwayDegree and MaxSwayDegree * -1 otherwise our gun would not stop swaying
	InterpDifference = UKismetMathLibrary::MakeRotator(UKismetMathLibrary::FClamp(Roll, MaxSwayDegree * -1, MaxSwayDegree),
		UKismetMathLibrary::FClamp(Pitch, MaxSwayDegree * -1, MaxSwayDegree), UKismetMathLibrary::FClamp(Yaw, MaxSwayDegree * -1, MaxSwayDegree));
	Weapon->SetRelativeRotation(InterpDifference);
}

void ABSc3bController::Client_ShowHitmarker_Implementation(FName HitBone)
{
	if (PlayerHUD)
	{
		PlayerHUD->ShowHitmarker();  //Show Hitmarker image
		if (HitBone == "head")  //If we have hit the head bone
		{
			if (HeadshotSound)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), HeadshotSound);	
			}
			
		} else
		{
			if (PlayerHitmarkerSound)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), PlayerHitmarkerSound);
			}
		}
		
	}
}

void ABSc3bController::ShowGameOverMessage(FString WinningPlayerID)
{
	PlayerHUD->ShowGameOverMessage(WinningPlayerID);
}

void ABSc3bController::OnNetCleanup(UNetConnection* Connection)
{
	//Logout Player from the session inside EOS otherwise it will not know when a client has left
	if (GetLocalRole() == ROLE_Authority && PlayerState != NULL)
	{
		UEOS_GameInstance* GameInstanceRef = Cast<UEOS_GameInstance>(GetWorld()->GetGameInstance());
		/*if (GameInstanceRef)
		{
			GameInstanceRef->DestroySession();
		}
		*/
		ABSc3bGameMode* GameMode = Cast<ABSc3bGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			GameMode->PreLogout(this);
		}
	}
	Super::OnNetCleanup(Connection);
}

