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
	LongRange = 2,
	Silencer = 3
};

UENUM(BlueprintType)
enum EAttachmentType : int
{
	Scope = 0,
	Grip = 1,
	Muzzle = 2
};

USTRUCT(BlueprintType)
struct FAttachmentMesh
{
	GENERATED_BODY()
	
public:

	FAttachmentMesh()
	{
		Type = Scope;
		Mesh = nullptr;
	}
	
	FAttachmentMesh(EAttachmentType type, UStaticMesh* mesh)
	{
		Type = type;
		Mesh = mesh;
	}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EAttachmentType> Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMesh* Mesh;
};

class AAttachment;

UCLASS()
class BSC3B_API UWeapon : public USkeletalMeshComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attachment, meta = (AllowPrivateAccess = true))
	TSubclassOf<AAttachment> AttachmentActor;

	UPROPERTY()
	AAttachment* ScopeActor;

	UPROPERTY()
	AAttachment* MuzzleActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attachment, meta = (AllowPrivateAccess = true))
	UStaticMesh* MagazineMesh;

public:
	UWeapon();

	//EAttachmentKey Attachment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attachments)
	TMap<TEnumAsByte<EAttachmentKey>, FAttachmentMesh> Attachments;

	UFUNCTION()
	void EquipAttachment(EAttachmentKey Attachment);

	UPROPERTY()
	AAttachment* MagActor;

	UFUNCTION()
	void SpawnMag(FName SocketName);

	UFUNCTION()
	void UpdateMagTransform(FTransform Transform);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
