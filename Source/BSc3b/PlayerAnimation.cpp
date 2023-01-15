// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimation.h"

#include "BSc3bCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UPlayerAnimation::UPlayerAnimation()
{
	//default values
	Pitch = 0;
	LeftHand_FABRIK_Alpha = 1;
}

void UPlayerAnimation::FootStep_Notify()
{
	OwningPlayer->Server_PlayFootstep(OwningPlayer->GetActorLocation());
}

void UPlayerAnimation::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	APawn* OwningPawn = TryGetPawnOwner();
	if (IsValid(OwningPawn))
	{
		OwningPlayer = Cast<ABSc3bCharacter>(OwningPawn);
		
		//Setting animation values that handle state machine poses
		//Have to do this as animation values are not automatically replicated
		Pitch = OwningPlayer->PlayerPitch;
		PlayerSpeed = OwningPlayer->GetVelocity().Length();
		bPlayerAiming = OwningPlayer->bIsPlayerAiming;
		PlayerXVelocity = OwningPlayer->PlayerVerticalVelocity;
		PlayerYVelocity = OwningPlayer->PlayerHorizontalVelocity;
		bIsPlayerDead = OwningPlayer->bIsDead;
		bPlayerShoot = OwningPlayer->bIsShooting;
		
		//Has to be above maximum player velocity when walking
		if (OwningPlayer->GetVelocity().Length() > 250 && PlayerXVelocity > 0)
		{
			bPlayerSprinting = true;
		}
		else
		{
			bPlayerSprinting = false;
		}

		//Setup LeftHand_Transform to work with FABRIK
		FTransform PlayerWeapon = OwningPlayer->GetWeaponTransform(TEXT("LeftHandSocket"), RTS_World);
		//Values set in Transformation function
		FVector OutPos;
		FRotator OutRot;
		OwningPlayer->GetMesh()->TransformToBoneSpace(TEXT("hand_r"), PlayerWeapon.GetLocation(), PlayerWeapon.GetRotation().Rotator(), OutPos, OutRot);
		LeftHand_Transform.SetLocation(OutPos);
		LeftHand_Transform.SetRotation(OutRot.Quaternion());
	}
}
