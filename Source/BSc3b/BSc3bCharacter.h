// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Bullet.h"
#include "NiagaraComponent.h"
#include "BSc3bCharacter.generated.h"

//Forward Declarations


UCLASS(config=Game)
class ABSc3bCharacter : public ACharacter
{
	GENERATED_BODY()
	
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapons, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Weapon1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapons, meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* LaserSight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapons, meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* LaserImpact;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveForward;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Shoot Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShootAction;

public:
	ABSc3bCharacter();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<ABullet> SpawnObject;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	float Health;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	float PlayerPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LaserDistance;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Health();
	bool Server_Health_Validate();
	void Server_Health_Implementation();

	UFUNCTION()
	void SpawnBullet(FVector Location, FRotator Rotation);

	//used by animation instance to get the weapons transform
	UFUNCTION()
	FTransform GetWeaponTransform(FName Socket, ERelativeTransformSpace TransformSpace);

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for Shooting input*/
	UFUNCTION()
	void Shoot(const FInputActionValue& Value);

	UFUNCTION()
	void OrientLaserSight();

	UFUNCTION()
	void SetPlayerPitchForOffset();

	UFUNCTION()
	void WeaponSway(float DeltaTime);

	//RPC for setting the player pitch to work with aim offset animation
	//Set it unreliable as we dont need the animation changes instantly
	UFUNCTION(Server, Unreliable)
	void Server_SetPlayerPitchForOffset();
	void Server_SetPlayerPitchForOffset_Implementation();

	//RPC for Shoot
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Shoot(FVector Location, FRotator Rotation);
	bool Server_Shoot_Validate(FVector Location, FRotator Rotation);
	void Server_Shoot_Implementation(FVector Location, FRotator Rotation);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	//Function to call whenever I would change the health function. Can check health value for example
	UFUNCTION()
	void OnRep_Health();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector2D LookAxisVector;


public:

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

