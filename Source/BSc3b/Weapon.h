// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapon.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum EAttachmentKey : int
{
	None = 0,
	RedDot = 1,
	LongRange = 2
};


UCLASS()
class BSC3B_API UWeapon : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attachment, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* Scope;

public:
	UWeapon();

	EAttachmentKey Attachment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attachments)
	TMap<TEnumAsByte<EAttachmentKey>, UStaticMesh*> Attachments;

	virtual void BeginPlay() override;
};
