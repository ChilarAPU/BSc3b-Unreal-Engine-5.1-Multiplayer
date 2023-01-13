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

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Shoot Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShootAction;
	
	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AimAction;

public:
	ABSc3bCharacter();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<ABullet> SpawnObject;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	float Health;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	float PlayerPitch;

	//Two variables used to handle walking blend space
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	float PlayerHorizontalVelocity;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	float PlayerVerticalVelocity;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool bIsPlayerAiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LaserDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundBase* Footstep;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundAttenuation* FootstepAttenuation;

	/* Called by shoot() to make sure any health changes are always
	 * done on the server. This is to prevent clients from being able to
	 * change their health client side
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Health();
	bool Server_Health_Validate();
	void Server_Health_Implementation();

	/* Called when a player is respawned. Currently only
	 * resets the laser sight to be visible. Without this, server Client
	 * does not show its laser sight on respawn
	 */
	UFUNCTION(Client, Reliable)
	void Client_Respawn();
	void Client_Respawn_Implementation();

	/* The only logic of this function is to move ourselves to the
	 * server so we can then successfully call a multicast RPC. Location
	 * gets passed through the animation notify event
	 */
	UFUNCTION(Server, Unreliable)
	void Server_PlayFootstep(FVector Location);
	void Server_PlayFootstep_Implementation(FVector Location);

	UFUNCTION(NetMulticast, Unreliable)
	void Multi_PlayFootstep(FVector Location);
	void Multi_PlayFootstep_Implementation(FVector Location);

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
	void Aim(const FInputActionValue& Value);

	/*Create line trace for laser sight which acts as the collider. Spawn in
	 * impact particle if laser is obstructed before reaching its end, otherwise
	 * continue on until end
	 */
	UFUNCTION()
	void OrientLaserSight();

	/*Transfer players pitch from a float to a usable degree value, which is then clamped
	 * and used inside the animation state machine to run the aim offset
	 */
	UFUNCTION()
	void SetPlayerPitchForOffset();

	/*Procedurally interpolate the weapon rotation when the player moves the camera
	 * to give the effect of the weapon swaying around. ALl rotation is done in
	 * object space so at to avoid any unaccounted negative values. 
	 */
	UFUNCTION()
	void WeaponSway(float DeltaTime);

	/*Server RPC for calling SetPlayerPitchForOffset().
	* Set it unreliable as we dont need the animation changes instantly
	*/
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

	//Make the look axis value global as it needs to be accessed from other functions
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector2D LookAxisVector;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector2D MoveAxisVector;


public:

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

