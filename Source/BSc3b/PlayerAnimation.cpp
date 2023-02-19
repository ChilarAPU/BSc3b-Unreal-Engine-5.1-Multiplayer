// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimation.h"

#include "BSc3bCharacter.h"
#include "Kismet/GameplayStatics.h"

UPlayerAnimation::UPlayerAnimation()
{
	//default values
	Pitch = 0;
	LeftHand_FABRIK_Alpha = 1;
}

void UPlayerAnimation::FootStep_Notify()
{
	WalkAttenuation->Attenuation.FalloffDistance = 500;
	if (OwningPlayer->IsLocallyControlled())
	{
		//multicast function that passes through our owners current location so it works correctly with attenuation
		OwningPlayer->Server_PlayFootstep(OwningPlayer->GetActorLocation(), WalkFootstep, WalkAttenuation);
	}
}

void UPlayerAnimation::Shoot_Notify(bool bAiming)
{
	//Run all shooting logic ending with the bullet being spawned into the world with an impulse
	OwningPlayer->ShootLogic(bAiming);
}

void UPlayerAnimation::EndReload_Notify()
{
	if (OwningPlayer->HasAuthority())
	{
		OwningPlayer->bReloading = false;
		OwningPlayer->Ammo = 30;
	}
	else if (OwningPlayer->IsLocallyControlled())
	{
		OwningPlayer->Server_Reload(false);
	}
	//Client only cloth sound
	OwningPlayer->SpawnClothSound(.3);
}

void UPlayerAnimation::AttachMag_Notify()
{
	if (IsValid(MagAttach))
	{
		//Do want to attenuate this later on in the project
		UGameplayStatics::PlaySound2D(GetWorld(), MagAttach);
		
	}
	OwningPlayer->ToggleMagazineVisibility(false);
}

void UPlayerAnimation::DetachMag_Notify()
{
	if (IsValid(MagDetach))
	{
		//Do want to attenuate this later on in the project
		UGameplayStatics::PlaySound2D(GetWorld(), MagDetach);
	}
	
	OwningPlayer->ToggleMagazineVisibility(true);
}

void UPlayerAnimation::JogFootstep_Notify()
{
	WalkAttenuation->Attenuation.FalloffDistance = 700;
	if (OwningPlayer->IsLocallyControlled())
	{
		//multicast function that passes through our owners current location so it works correctly with attenuation
		OwningPlayer->Server_PlayFootstep(OwningPlayer->GetActorLocation(), JogFootstep, WalkAttenuation);	
	}
	
}

void UPlayerAnimation::RunFootstep_Notify()
{
	WalkAttenuation->Attenuation.FalloffDistance = 900;
	if (OwningPlayer->IsLocallyControlled())
	{
		//multicast function that passes through our owners current location so it works correctly with attenuation
		OwningPlayer->Server_PlayFootstep(OwningPlayer->GetActorLocation(), RunFootstep, WalkAttenuation);	
	}
}

void UPlayerAnimation::EndOfHit_Notify()
{
	//Tell our state machine to stop running our current hit animation
	OwningPlayer->Server_EndHit();
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
		if (IsValid(OwningPlayer))
		{
			Pitch = OwningPlayer->PlayerPitch;
			PlayerSpeed = OwningPlayer->GetVelocity().Length();
			bPlayerAiming = OwningPlayer->bIsPlayerAiming;
			PlayerXVelocity = OwningPlayer->PlayerVerticalVelocity;
			PlayerYVelocity = OwningPlayer->PlayerHorizontalVelocity;
			bIsPlayerDead = OwningPlayer->bIsDead;
			bPlayerShoot = OwningPlayer->bIsShooting;
			bHit = OwningPlayer->bHitByBullet;
			bPlayerReload = OwningPlayer->bReloading;
			bPlayerChangingAttachments = OwningPlayer->bIsChangingAttachments;
			//End of state machine value setting
            		
			//Has to be above maximum player velocity when walking
			//Set sprinting animation to play based on player velocity and not a boolean from a players input
			if (OwningPlayer->GetVelocity().Length() > 350 && PlayerXVelocity > 0)
			{
				bPlayerSprinting = true;
            	}
			else
			{
				bPlayerSprinting = false;
			}
            
			//Stop our Left hand IK temporarily when playing the reload animation
			if (bPlayerReload)
			{
				LeftHand_FABRIK_Alpha = 0;
			}
			else
			{
				LeftHand_FABRIK_Alpha = 1;
			}
            		
            
			//Setup LeftHand_Transform to work with FABRIK
			FTransform PlayerWeapon = OwningPlayer->GetWeaponTransform(TEXT("LeftHandSocket"), RTS_World);
			//Values set in Transformation function
			FVector OutPos;
			FRotator OutRot;
			//Get the location of our socket in relation to our right hand in bone space
			OwningPlayer->GetMesh()->TransformToBoneSpace(TEXT("hand_r"), PlayerWeapon.GetLocation(), PlayerWeapon.GetRotation().Rotator(), OutPos, OutRot);
            	LeftHand_Transform.SetLocation(OutPos);
			LeftHand_Transform.SetRotation(OutRot.Quaternion());
            
			//Will be moved but updates the location of our separate magazine mesh for the reload animation
			//OwningPlayer->UpdateMagazineTransform();
		}
		
	}
}
