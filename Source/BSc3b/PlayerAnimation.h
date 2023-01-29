// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimation.generated.h"

//Forward Declarations
class ABSc3bCharacter;


UCLASS()
class BSC3B_API UPlayerAnimation : public UAnimInstance
{
	GENERATED_BODY()

	UPlayerAnimation();
public:

	////// VALUES SET BY PLAYER TO HANDLE STATE MACHINE //////
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Pitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPlayerAiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerXVelocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerYVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPlayerDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPlayerShoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPlayerSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPlayerReload;

	////// FABRIK VARIABLES //////
	//1 for attach, 0 for detach
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LeftHand_FABRIK_Alpha;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform LeftHand_Transform;

	UPROPERTY()
	ABSc3bCharacter* OwningPlayer;

	UFUNCTION(BlueprintCallable)
	void FootStep_Notify();

	UFUNCTION(BlueprintCallable)
	void Shoot_Notify(bool bAiming);

	UFUNCTION(BlueprintCallable)
	void EndReload_Notify();

	UFUNCTION(BlueprintCallable)
	void AttachMag_Notify();

	UFUNCTION(BlueprintCallable)
	void DetachMag_Notify();
	
	UFUNCTION(BlueprintCallable)
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
