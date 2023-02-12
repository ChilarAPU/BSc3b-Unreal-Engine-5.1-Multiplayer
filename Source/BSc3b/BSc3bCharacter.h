// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Bullet.h"
#include "NiagaraComponent.h"
#include "BSc3bCharacter.generated.h"

enum EAttachmentKey : int;
class ABSc3bController;
//Forward Declarations
class UCameraComponent;
class UWeapon;
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
	UWeapon* Weapon;

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

	/** Attachment Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttachmentAction;

	/** Menu Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MenuAction;

	////// PRIVATE VARIABLES //////

	/* Easily adjust how long the laser should travel */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Laser, meta = (AllowPrivateAccess = "true"))
	float LaserDistance;

	/* Set when movement key is pressed while sprinting other than forwards. This stops the player
	 * from continuing to sprint in directions they shouldn't
	 */
	UPROPERTY(EditAnywhere)
	bool bStopSprinting;

	/* Used to stop our sprinting mechanic from breaking when it is pressed while aiming
	 * then released after aiming has ceased */
	UPROPERTY()
	bool bWasAimingCanceled;

	/* Used to determine whether or the aim input was pressed during input. This is processed during the aim
	 * event to stop unwanted movement changes from occuring.
	 */
	UPROPERTY()
	bool bAimingWhileSprinting;

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

	/* Replicate whether player is dead to animation */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool bIsDead;

	/* Replicate whether player is shooting to animation */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool bIsShooting;
	
	/* Replicate whether player is sprinting to animation */
	UPROPERTY(Replicated, BlueprintReadOnly, EditAnywhere)
	bool bIsSprinting;

	/* Replicate whether player has been hit by a bullet */
	UPROPERTY(Replicated, BlueprintReadOnly, EditAnywhere)
	bool bHitByBullet;

	/* Replicate whether player is reloading */
	UPROPERTY(Replicated, BlueprintReadOnly, EditAnywhere)
	bool bReloading;
	
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
	void Server_PlayFootstep(FVector Location, USoundBase* Sound, USoundAttenuation* Attenuation);
	void Server_PlayFootstep_Implementation(FVector Location, USoundBase* Sound, USoundAttenuation* Attenuation);

	/* Only functionality is to tell our state machine to start the reload animation*/
	UFUNCTION(Server, Reliable)
	void Server_Reload(bool Reload);
	void Server_Reload_Implementation(bool Reload);
	
	////// LOGIC FUNCTION NEEDED TO BE ACCESSED OUTSIDE OF CURRENT CLASS //////
	/* Used by animation instance to get the weapons transform */
	UFUNCTION()
	FTransform GetWeaponTransform(FName Socket, ERelativeTransformSpace TransformSpace);

	/* Called by shoot notify event that spawns in a bullet at the guns laser sight location*/
	UFUNCTION()
	void ShootLogic(bool bAimingIn);

	/* Called by widget to equip an attachment. Just an access through function so the component can stay
	 * private
	 */
	UFUNCTION()
	void EquipWeaponAttachment(EAttachmentKey Attachment);

	/* Spawn a new magazine mesh in our hand while simultaneously hiding the magazine that is attached to our weapon */
	UFUNCTION()
	void ToggleMagazineVisibility(bool Hide);

	/* Called by animation to update the transform of our magazine. NOTE: IMRPOVE BY ATTACHING TO SOCKET INSTEAD OF
	 * SETTING LOCATION EVERY FRAME
	 */
	UFUNCTION()
	void UpdateMagazineTransform();

	/* Wrapper function to call Multicast_EquipWeaponAttachment()*/
	UFUNCTION(Server, Unreliable)
	void Server_EquipWeaponAttachment(EAttachmentKey Attachment);
	void Server_EquipWeaponAttachment_Implementation(EAttachmentKey Attachment);

	/* Equip an attachment for other clients missing out our owning client */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_EquipWeaponAttachment(EAttachmentKey Attachment);
	void Multicast_EquipWeaponAttachment_Implementation(EAttachmentKey Attachment);

	/* Spawn weapon attachment actors on the server. This also attaches these actors to specific sockets on the weapon*/
	UFUNCTION(Server, Unreliable)
	void Server_Spawn();
	void Server_Spawn_Implementation();

	/* Called at the end of hit animation and simply returns our boolean to false*/
	UFUNCTION(Server, Unreliable)
	void Server_EndHit();
	void Server_EndHit_Implementation();

	/* Spawn in a new character class and posses them. Also run functionality needed before posses can happen */
	UFUNCTION(Server, Reliable)
	void Server_Respawn();
	void Server_Respawn_Implementation();

	/* Called when Respawn button is pressed and simply hides the button again */
	UFUNCTION(Client, Reliable)
	void Client_ResetInput();
	void Client_ResetInput_Implementation();

	/* Have to call a custom begin play function as the server client does not
	 * run BeginPlay() for some reason when spawning in a new character
	 */
	UFUNCTION(Client, Reliable)
	void Client_CustomBeginPlay();
	void Client_CustomBeginPlay_Implementation();

	/* Holds the ammo for the current gun */
	UPROPERTY(Replicated)
	int Ammo;

	/* Determines whether the player is inside the attachment changing pose. Used to stop other inputs from
	 * passing
	 */
	UPROPERTY()
	bool bIsChangingAttachments;

	/* Spawn a 2D cloth sound at a random time inside the sound file along with adjusting the pitch between
	 * a value of 1 - 4
	 */
	UFUNCTION()
	void SpawnClothSound(float Duration);

	UPROPERTY()
	ABSc3bController* PlayerController;

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

	/** Called for Reloading input*/
	UFUNCTION()
	void OpenAttachments(const FInputActionValue& Value);

	/** Called for Menu input*/
	UFUNCTION()
	void OpenInGameMenu(const FInputActionValue& Value);

	/* Used inside menu input event to dictate whether we need to open or close the widget */
	UPROPERTY()
	bool IsMenuOpen;
	
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
	
	/* Simple function that shows the respawn button when a player has been killed. Can also include other
	 * client specific functionality that can happen upon death
	 */
	UFUNCTION(Client, Reliable)
	void Client_Respawn();
	void Client_Respawn_Implementation();
	
	////// PROTECTED MULTICAST RPCS //////
	/* Called by our Server footstep RPC to make sure every client will hear this footstep.
	 * Has to be called by the server otherwise it acts as a client RPC
	 */
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_PlayFootstep(FVector Location, USoundBase* Sound, USoundAttenuation* Attenuation);
	void Multi_PlayFootstep_Implementation(FVector Location, USoundBase* Sound, USoundAttenuation* Attenuation);

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

