// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Bullet.h"
#include "NiagaraComponent.h"
#include "BSc3bCharacter.generated.h"

class ABSc3bController;
//Forward Declarations
class UCameraComponent;
class USkeletalMeshComponent;
class UNiagaraComponent;
class UInputMappingContext;
class UInputAction;
class USoundBase;
class USoundAttenuation;
class UInputComponent;
class UPlayerHUD;

UCLASS(config=Game)
class ABSc3bCharacter : public ACharacter
{
	GENERATED_BODY()

	////// PRIVATE COMPONENTS //////
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapons, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Weapon1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapons, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* LaserSight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapons, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* LaserImpact;

	////// INPUT ACTIONS //////
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Shoot Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShootAction;
	
	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;
	
	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

	////// PRIVATE VARIABLES //////
	/* Actor which should spawn when the player shoot a weapon */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Bullet, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABullet> SpawnObject;

	/* Easily adjust how long the laser should travel */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Laser, meta = (AllowPrivateAccess = "true"))
	float LaserDistance;

	/* Footstep sound that gets played through animation notify event */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	USoundBase* Footstep;

	/* Sound attenuation for footsteps or anything with similar volume level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	USoundAttenuation* FootstepAttenuation;

	/* Cloth sound that plays whenever the player aims in to replicate the sound of clothes moving */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	USoundBase* AimSound;

	/* Set when movement key is pressed while sprinting other than forwards. This stops the player
	 * from continuing to sprint in directions they shouldn't
	 */
	UPROPERTY(EditAnywhere)
	bool bStopSprinting;

	UPROPERTY()
	bool bWasAimingCanceled;

	UPROPERTY()
	ABSc3bController* PlayerController;

public:
	ABSc3bCharacter();

	////// PUBLIC REPLICATED VARIABLES //////
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	float Health;

	////// REPLICATED ANIMATION SPECIFIC VARIABLES //////
	/* Replicate Player pitch to aim offset */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	float PlayerPitch;

	/* Replicate Player sideways velocity to handle which direction the legs should move */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	float PlayerHorizontalVelocity;

	/* Replicate Player forward velocity to handle which direction the legs should move */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	float PlayerVerticalVelocity;

	/* Replicate Player aiming input value */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool bIsPlayerAiming;

	/* Replicate whether player is dead or not to animation */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool bIsDead;

	/* Replicate whether player is shooting or not to animation */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool bIsShooting;

	/* Replicate whether player is sprinting or not to animation */
	UPROPERTY(Replicated, BlueprintReadOnly, EditAnywhere)
	bool bIsSprinting;

	UPROPERTY(Replicated, BlueprintReadOnly, EditAnywhere)
	bool bHitByBullet;
	
	////// PUBLIC SERVER RPCS //////
	/* Called by shoot() to make sure any health changes are always
	 * done on the server. This is to prevent clients from being able to
	 * change their health client side
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Health(FName Bone);
	bool Server_Health_Validate(FName Bone);
	void Server_Health_Implementation(FName Bone);

	/* The only logic of this function is to move ourselves to the
	 * server so we can then successfully call a multicast RPC. Location
	 * gets passed through the animation notify event
	 */
	UFUNCTION(Server, Unreliable)
	void Server_PlayFootstep(FVector Location);
	void Server_PlayFootstep_Implementation(FVector Location);
	
	////// LOGIC FUNCTION NEEDED TO BE ACCESSED OUTSIDE OF CURRENT CLASS //////
	/* Used by animation instance to get the weapons transform */
	UFUNCTION()
	FTransform GetWeaponTransform(FName Socket, ERelativeTransformSpace TransformSpace);

	/* Called by shoot notify event that spawns in a bullet at the guns laser sight location*/
	UFUNCTION()
	void ShootLogic(bool bAimingIn);

protected:

	////// INPUT FUNCTIONS //////
	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	
	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for Shooting input*/
	UFUNCTION()
	void Shoot(const FInputActionValue& Value);

	/* Called once shoot button is let go and resets our replicated values*/
	UFUNCTION()
	void ShootComplete(const FInputActionValue& Value);

	/** Called for Aiming input*/
	UFUNCTION()
	void Aim(const FInputActionValue& Value);

	/** Called for Sprinting input*/
	UFUNCTION()
	void Sprint(const FInputActionValue& Value);

	/** Called for Reloading input*/
	UFUNCTION()
	void Reload(const FInputActionValue& Value);
	
	
	////// PROTECTED LOGIC FUNCTIONS //////
	/*Create line trace for laser sight which acts as the collider. Spawn in
	 * impact particle if laser is obstructed before reaching its end, otherwise
	 * continue on until end */
	UFUNCTION()
	void OrientLaserSight();

	/*Transfer players pitch from a float to a usable degree value, which is then clamped
	 * and used inside the animation state machine to run the aim offset */
	UFUNCTION()
	void SetPlayerPitchForOffset();

	/*Procedurally interpolate the weapon rotation when the player moves the camera
	 * to give the effect of the weapon swaying around. ALl rotation is done in
	 * object space so at to avoid any unaccounted negative world space values */
	UFUNCTION()
	void WeaponSway(float DeltaTime);

	/* Logic function for spawning in a bullet at any location/rotation we pass through assigning
	* the correct owner for the actor, as well as adding an impulse to the bullet */
	UFUNCTION()
	void SpawnBullet(FVector Location, FRotator Rotation);

	////// PROTECTED SERVER RPCS //////

	//RPC for Shoot
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Shoot(FVector Location, FRotator Rotation);
	bool Server_Shoot_Validate(FVector Location, FRotator Rotation);
	void Server_Shoot_Implementation(FVector Location, FRotator Rotation);

	////// ANIMATION REPLICATION SERVER RPCS //////
	/*Server RPC for calling SetPlayerPitchForOffset().
	* Set it unreliable as we dont need the animation changes instantly */
	UFUNCTION(Server, Unreliable)
	void Server_SetPlayerPitchForOffset();
	void Server_SetPlayerPitchForOffset_Implementation();
	
	UFUNCTION(Server, Reliable)
	void Server_PlayerVelocity(FVector2D MovementVector);
	void Server_PlayerVelocity_Implementation(FVector2D MovementVector);

	UFUNCTION(Server, Reliable)
	void Server_PlayerAiming(bool bIsAiming, float speed);
	void Server_PlayerAiming_Implementation(bool bIsAiming, float speed);

	UFUNCTION(Server, Reliable)
	void Server_PlayerShooting(bool bShooting);
	void Server_PlayerShooting_Implementation(bool bShooting);

	UFUNCTION(Server, Reliable)
	void Server_PlayerSprinting(bool Sprinting, float Speed);
	void Server_PlayerSprinting_Implementation(bool Sprinting, float Speed);

	////// PROTECTED CLIENT RPCS //////
	/* Called whenever we aim in and out of our weapon to toggle the visibility of our
	 * laser sight
	 */
	UFUNCTION(Client, Reliable)
	void Client_FlipLaserVisibility(bool Visible);
	void Client_FlipLaserVisibility_Implementation(bool Visible);

	UFUNCTION(Client, Reliable)
	void Client_Respawn();
	void Client_Respawn_Implementation();
	////// PROTECTED MULTICAST RPCS //////
	/* Called by our Server footstep RPC to make sure every client will hear this footstep.
	 * Has to be called by the server otherwise it acts as a client RPC
	 */
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_PlayFootstep(FVector Location);
	void Multi_PlayFootstep_Implementation(FVector Location);

	////// REP NOTIFY FUNCTIONS //////
	//Function to call whenever I would change the health function. Can check health value for example
	UFUNCTION()
	void OnRep_Health();

	////// PROTECTED VARIABLES //////
	//Make the look axis value global as it needs to be accessed from setting pitch functions
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector2D LookAxisVector;

	////// PROTECTED REPLICATED VARIABLES //////
	/* Used with logic to decide the values of the two floats above */
	UPROPERTY(Replicated, BlueprintReadOnly, EditAnywhere)
	FVector2D MoveAxisVector;

	////// OVERRIDEN DEFAULT CLASS FUNCTIONS //////
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

	/* All replicated values must be called in this function with any conditions */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	
};

