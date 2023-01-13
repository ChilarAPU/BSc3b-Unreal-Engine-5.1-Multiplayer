// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimation.generated.h"

/**
 * 
 */
UCLASS()
class BSC3B_API UPlayerAnimation : public UAnimInstance
{
	GENERATED_BODY()

	UPlayerAnimation();
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Pitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPlayerAiming;

	//1 for attach, 0 for detach
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LeftHand_FABRIK_Alpha;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform LeftHand_Transform;

	UPROPERTY()
	class ABSc3bCharacter* OwningPlayer;

	UFUNCTION(BlueprintCallable)
	void FootStep_Notify();

	UFUNCTION(BlueprintCallable)
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
