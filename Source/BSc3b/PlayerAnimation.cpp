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
	OwningPlayer->Server_PlayFootstep(OwningPlayer->GetActorLocation(), WalkFootstep, WalkAttenuation);
}

void UPlayerAnimation::Shoot_Notify(bool bAiming)
{
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
	OwningPlayer->SpawnClothSound(.3);
}

void UPlayerAnimation::AttachMag_Notify()
{
	if (IsValid(MagAttach))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), MagAttach);
	}
	OwningPlayer->ToggleMagazineVisibility(false);
}

void UPlayerAnimation::DetachMag_Notify()
{
	if (IsValid(MagDetach))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), MagDetach);
	}
	
	OwningPlayer->ToggleMagazineVisibility(true);
}

void UPlayerAnimation::JogFootstep_Notify()
{
	WalkAttenuation->Attenuation.FalloffDistance = 700;
	OwningPlayer->Server_PlayFootstep(OwningPlayer->GetActorLocation(), JogFootstep, WalkAttenuation);
}

void UPlayerAnimation::RunFootstep_Notify()
{
	WalkAttenuation->Attenuation.FalloffDistance = 900;
	OwningPlayer->Server_PlayFootstep(OwningPlayer->GetActorLocation(), RunFootstep, WalkAttenuation);
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
		bHit = OwningPlayer->bHitByBullet;
		bPlayerReload = OwningPlayer->bReloading;
		bPlayerChangingAttachments = OwningPlayer->bIsChangingAttachments;
		
		//Has to be above maximum player velocity when walking
		if (OwningPlayer->GetVelocity().Length() > 350 && PlayerXVelocity > 0)
		{
			bPlayerSprinting = true;
		}
		else
		{
			bPlayerSprinting = false;
		}

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
		OwningPlayer->GetMesh()->TransformToBoneSpace(TEXT("hand_r"), PlayerWeapon.GetLocation(), PlayerWeapon.GetRotation().Rotator(), OutPos, OutRot);
		LeftHand_Transform.SetLocation(OutPos);
		LeftHand_Transform.SetRotation(OutRot.Quaternion());

		OwningPlayer->UpdateMagazineTransform();
	}
}
