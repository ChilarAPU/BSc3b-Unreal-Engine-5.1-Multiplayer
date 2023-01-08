// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimation.h"

#include "BSc3bCharacter.h"

UPlayerAnimation::UPlayerAnimation()
{
	//default values
	Pitch = 0;
	LeftHand_FABRIK_Alpha = 1;
}

void UPlayerAnimation::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	APawn* OwningPawn = TryGetPawnOwner();
	if (IsValid(OwningPawn))
	{
		ABSc3bCharacter* OwningPlayer = Cast<ABSc3bCharacter>(OwningPawn);
		//Animation usable value for Pitch
		Pitch = OwningPlayer->PlayerPitch;
		FTransform PlayerWeapon = OwningPlayer->GetWeaponTransform(TEXT("LeftHandSocket"), RTS_World);
		//Values set in Transformation function
		FVector OutPos;
		FRotator OutRot;
		OwningPlayer->GetMesh()->TransformToBoneSpace(TEXT("hand_r"), PlayerWeapon.GetLocation(), FRotator(0.0, 0.0, 0.0), OutPos, OutRot);
		LeftHand_Location.SetLocation(OutPos);
	}
}
