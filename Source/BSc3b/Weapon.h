// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapon.generated.h"

/**
 * 
 */

class ABSc3bCharacter;
/* Holds all available attachments that can be placed on the weapon*/
UENUM(BlueprintType)
enum EAttachmentKey : int
{
	None = 0,
	RedDot = 1,
	LongRange = 2,
	Silencer = 3,
	ForeGrip = 4,
	SciFiScope = 5,
	SlantedGrip = 6,
	MakarovSilencer = 7,
	SilencerTwo = 8
};

/* Holds all available attachment types that are available on the weapon*/
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
	/* What attachment type should be assigned to this struct */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EAttachmentType> Type;

	/* What mesh should be assigned to this struct */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMesh* Mesh;

	/* Statistic modifiers for this value in the struct */
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

	/* Attachment actor reference*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attachment, meta = (AllowPrivateAccess = true))
	TSubclassOf<AAttachment> AttachmentActor;

	/* All attachment types associated actors which are always spawned with the weapon */
	UPROPERTY(Replicated)
	AAttachment* ScopeActor;

	UPROPERTY(Replicated)
	AAttachment* MuzzleActor;

	UPROPERTY(Replicated)
	AAttachment* GripActor;

	/* Mesh for our magazine that replaces original one on weapon during reload animation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attachment, meta = (AllowPrivateAccess = true))
	UStaticMesh* MagazineMesh;

	/* Handles spawning a new attachment mesh along with adjusting the statistic parameters.
	 * This gets called through a button press on the players widget. */
	UFUNCTION()
	void SetAttachmentMesh(AAttachment* Actor, EAttachmentKey Attachment, TEnumAsByte<EAttachmentKey>& CachedAttachment);

	/* Holds the old attachment key for each type. This is used in SetAttachmentMesh() to remove old attachments
	 * statistics before adding on the new ones. 
	 */
	UPROPERTY()
	TEnumAsByte<EAttachmentKey> CachedScopeKey;

	UPROPERTY()
	TEnumAsByte<EAttachmentKey> CachedMuzzleKey;

	UPROPERTY()
	TEnumAsByte<EAttachmentKey> CachedGripKey;

	/* TMap which holds all our attachments for the current weapon along with their associate values.
	 * Gets set in blueprints for ease of access. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attachments, meta = (AllowPrivateAccess))
	TMap<TEnumAsByte<EAttachmentKey>, FAttachmentMesh> Attachments;


public:
	UWeapon();

	/* Default statistic values for the weapon. These get adjusted through attachments and shown through a progress
	 * bar on the players HUD.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Statistics)
	float DamageStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Statistics)
	float RangeStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Statistics)
	float StabilityStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Statistics)
	float MobilityStat;

	/* Finds our attachment with the given enum key and ensures we spawn it inside the correct actor*/
	UFUNCTION()
	void EquipAttachment(EAttachmentKey Attachment);

	UPROPERTY()
	AAttachment* MagActor;

	/* Spawn our magazine mesh at the players left hand. Called by reload notify event*/
	UFUNCTION()
	void SpawnMag(FName SocketName, FName SocketAttachName, ABSc3bCharacter* Player);

	/* Updates our magazine transform. NOTE: SHOULD BE DEPRECATE WHEN ATTACH TO SOCKET IS IMPLEMENTED*/
	UFUNCTION()
	void UpdateMagTransform(FTransform Transform);

	/* Spawns the attachment type actors into the world for local client only as opposed to where the actors
	 * are spawned in the server through the player*/
	UFUNCTION()
	void SpawnAttachment();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

};
