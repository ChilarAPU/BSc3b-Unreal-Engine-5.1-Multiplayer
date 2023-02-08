// Fill out your copyright notice in the Description page of Project Settings.


#include "BSc3bController.h"

#include "BSc3bCharacter.h"
#include "PlayerAnimation.h"
#include "PlayerHUD.h"
#include "Weapon.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ABSc3bController::ABSc3bController()
{
	Player = nullptr;
}

void ABSc3bController::BeginPlay()
{
	Super::BeginPlay();
	//Cast to our character just in case we ever need it
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
	//Set our player animation class so we can adjust the values inside of it directly from our character
	if (!IsValid(PlayerAnimClass))
	{
		return;
	}
	PlayerAnim = PlayerAnimClass.GetDefaultObject();
	
}

void ABSc3bController::ShowRespawnButton(bool Visible)
{
	if (IsValid(PlayerHUD))
	{
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

