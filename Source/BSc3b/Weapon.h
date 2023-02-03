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
	Silencer = 3,
	ForeGrip = 4
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
		Damage = 0;
		Range = 0;
		Stability = 0;
		Mobility = 0;
	}
	
	FAttachmentMesh(EAttachmentType type, UStaticMesh* mesh, float damage = 0, float range = 0, float stability = 0, float mobility = 0)
	{
		Type = type;
		Mesh = mesh;
		Damage = damage;
		Range = range;
		Stability = stability;
		Mobility = mobility;
	}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EAttachmentType> Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMesh* Mesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Statistics, meta = (ClampMin = -5, ClampMax = 5, UIMin = -5, UIMax = 5))
	float Damage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Statistics, meta = (ClampMin = -5, ClampMax = 5, UIMin = -5, UIMax = 5))
	float Range;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Statistics, meta = (ClampMin = -5, ClampMax = 5, UIMin = -5, UIMax = 5))
	float Stability;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Statistics, meta = (ClampMin = -5, ClampMax = 5, UIMin = -5, UIMax = 5))
	float Mobility;
};

class AAttachment;

UCLASS()
class BSC3B_API UWeapon : public USkeletalMeshComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attachment, meta = (AllowPrivateAccess = true))
	TSubclassOf<AAttachment> AttachmentActor;

	UPROPERTY(Replicated)
	AAttachment* ScopeActor;

	UPROPERTY(Replicated)
	AAttachment* MuzzleActor;

	UPROPERTY(Replicated)
	AAttachment* GripActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attachment, meta = (AllowPrivateAccess = true))
	UStaticMesh* MagazineMesh;

	UFUNCTION()
	void SetAttachmentMesh(AAttachment* Actor, EAttachmentKey Attachment, TEnumAsByte<EAttachmentKey>& CachedAttachment);

	UPROPERTY()
	TEnumAsByte<EAttachmentKey> CachedScopeKey;

	UPROPERTY()
	TEnumAsByte<EAttachmentKey> CachedMuzzleKey;

	UPROPERTY()
	TEnumAsByte<EAttachmentKey> CachedGripKey;


public:
	UWeapon();

	//EAttachmentKey Attachment;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Statistics)
	float DamageStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Statistics)
	float RangeStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Statistics)
	float StabilityStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Statistics)
	float MobilityStat;

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

	UFUNCTION()
	void SpawnAttachment();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

};
