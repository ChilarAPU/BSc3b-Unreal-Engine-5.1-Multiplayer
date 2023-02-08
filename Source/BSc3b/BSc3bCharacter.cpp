	// Copyright Epic Games, Inc. All Rights Reserved.

#include "BSc3bCharacter.h"

#include "Attachment.h"
#include "BSc3bController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerAnimation.h"
#include "PlayerHUD.h"
#include "Weapon.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"


//////////////////////////////////////////////////////////////////////////
// ABSc3bCharacter

ABSc3bCharacter::ABSc3bCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	//GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	//GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a follow camera
	const FName CameraSocketName = TEXT("cameraSocket");
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(GetMesh(), CameraSocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm
	FollowCamera->SetRelativeLocation(FVector(-4.506538, 0.355924, -4.909522)); //Set Location of camera
	FollowCamera->SetFieldOfView(110);
	//Dont need to set rotation as the camera is attached to the head socket of mesh, overriding any rotation values

	//Create weapon static mesh component
	const FName WeaponSocketName = TEXT("weapon_handle");
	Weapon = CreateDefaultSubobject<UWeapon>(TEXT("Weapon New"));
	Weapon->SetupAttachment(GetMesh(), WeaponSocketName);

	const FName LaserSocketName = TEXT("b_gun_muzzleflash");
	LaserSight = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LaserSight"));
	LaserSight->SetupAttachment(Weapon, LaserSocketName);
	LaserSight->SetVisibility(false);
	LaserSight->SetIsReplicated(false);

	LaserImpact = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LaserImpact"));
	LaserImpact->SetupAttachment(Weapon);
	LaserImpact->SetVisibility(false);
	LaserImpact->SetIsReplicated(false);

	//Will Setup custom movement rotation as this rotates the mesh for any movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true;
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	//default values
	LaserDistance = 1000;
	Health = 100;
	OnRep_Health();
	PlayerPitch = 0;
	PlayerHorizontalVelocity = 0;
	PlayerVerticalVelocity = 0;
	bIsPlayerAiming = false;
	bIsDead = false;
	bIsShooting = false;
	bIsSprinting = false;
	bStopSprinting = false;
	bWasAimingCanceled = false;
	bHitByBullet = false;
	bIsChangingAttachments = false;
	bReloading = false;
	PlayerController = nullptr;
	Ammo = 30;
	
}

void ABSc3bCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	PlayerController = Cast<ABSc3bController>(Controller);
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
		Client_CustomBeginPlay();
		
		//Setting this here forces clients input to switch to the game on launch
		//not sure if this is a good feature or not
		/*PlayerController->SetShowMouseCursor(false);
		PlayerController->SetIgnoreLookInput(false);
		const FInputModeGameOnly Input;
		PlayerController->SetInputMode(Input);
		*/
	}
	/*if (HasAuthority())
	{
		Weapon->SpawnAttachment();
	} else if (IsLocallyControlled())
	{
		Server_Spawn();	
	}
	*/
	//Server_Spawn();
	
}

void ABSc3bCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsValid(Weapon))
	{
		OrientLaserSight();
		PlayerController->WeaponSway(DeltaSeconds, LookAxisVector, Weapon);
	}
	
	//Aim Offset replication code
	if (HasAuthority())
	{
		SetPlayerPitchForOffset();
	} else if (IsLocallyControlled())
	{
		SetPlayerPitchForOffset();
		Server_SetPlayerPitchForOffset();
	}
	PlayerController = Cast<ABSc3bController>(Controller);
	if (!PlayerController)
	{
		return;
	}
	if (IsValid(PlayerController->PlayerHUD))
	{
		PlayerController->PlayerHUD->HealthText = FString::SanitizeFloat(Health);
		PlayerController->PlayerHUD->AmmoCount = FString::SanitizeFloat(Ammo);
		//Call this function so we dont have to include the progress bar file
		PlayerController->PlayerHUD->AdjustStatPercentage(PlayerController->PlayerHUD->DamageStatBar, Weapon->DamageStat);
		PlayerController->PlayerHUD->AdjustStatPercentage(PlayerController->PlayerHUD->RangeStatBar, Weapon->RangeStat);
		PlayerController->PlayerHUD->AdjustStatPercentage(PlayerController->PlayerHUD->MobilityStatBar, Weapon->MobilityStat);
		PlayerController->PlayerHUD->AdjustStatPercentage(PlayerController->PlayerHUD->StabilityStatBar, Weapon->StabilityStat);
	}
}

void ABSc3bCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Tell it to replicate the variable all clients at all times
	DOREPLIFETIME(ABSc3bCharacter, Health);
	DOREPLIFETIME(ABSc3bCharacter, bHitByBullet);
	DOREPLIFETIME(ABSc3bCharacter, Ammo);
	
	//Replicated variables used in animations
	DOREPLIFETIME_CONDITION(ABSc3bCharacter, PlayerHorizontalVelocity, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABSc3bCharacter, PlayerVerticalVelocity, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABSc3bCharacter, bIsPlayerAiming, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABSc3bCharacter, MoveAxisVector, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABSc3bCharacter, PlayerPitch, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABSc3bCharacter, bIsShooting, COND_SkipOwner);
	DOREPLIFETIME(ABSc3bCharacter, bIsDead);
	DOREPLIFETIME(ABSc3bCharacter, bIsSprinting);
	DOREPLIFETIME(ABSc3bCharacter, bReloading);
}

void ABSc3bCharacter::OnRep_Health()
{
	
}

FTransform ABSc3bCharacter::GetWeaponTransform(FName Socket, ERelativeTransformSpace TransformSpace)
{
	if (IsValid(Weapon))
	{
		return Weapon->GetSocketTransform(Socket, TransformSpace);
	}
	//return an identity transform
	return FTransform();
	
}

	bool ABSc3bCharacter::Server_Shoot_Validate(FVector Location, FRotator Rotation)
{
	//Currently inside the server
	//Would be where cheat detection would be implemented.
		return true;
	//If this returns false, client who sent this RPC will be kicked out of the game
}

void ABSc3bCharacter::Server_Shoot_Implementation(FVector Location, FRotator Rotation)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnBullet(Location, Rotation);
	Multi_PlayFootstep(Location, PlayerController->Gunshot, PlayerController->GunshotAttenuation);
}

void ABSc3bCharacter::Server_PlayerVelocity_Implementation(FVector2D MovementVector)
{
	MoveAxisVector = MovementVector;
}

//////////////////////////////////////////////////////////////////////////
// Input
void ABSc3bCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABSc3bCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABSc3bCharacter::Look);

		//Shooting
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ABSc3bCharacter::Shoot);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &ABSc3bCharacter::ShootComplete);

		//Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ABSc3bCharacter::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ABSc3bCharacter::Aim);

		//Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ABSc3bCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABSc3bCharacter::Sprint);

		//Reloading
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ABSc3bCharacter::Reload);

		//Attachment System
		EnhancedInputComponent->BindAction(AttachmentAction, ETriggerEvent::Started, this, &ABSc3bCharacter::OpenAttachments);
		EnhancedInputComponent->BindAction(AttachmentAction, ETriggerEvent::Completed, this, &ABSc3bCharacter::OpenAttachments);

	}

}

bool ABSc3bCharacter::Server_Health_Validate(FName Bone)
{
	return true;
}

void ABSc3bCharacter::Server_Health_Implementation(FName Bone)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		Health -= 1;
		
		if (bHitByBullet)
		{
			bHitByBullet = false;
		} else
		{
			bHitByBullet = true;
		}
		
		
		if (Bone == "head")
		{
			Health = 0;
			UE_LOG(LogTemp, Warning, TEXT("HeadShot"));
		}
		//temporary way to spawn in new player character
		if (Health < 95)
		{
			bIsDead = true;
			Client_Respawn();
			//const FVector t = GetActorLocation();
			//const FRotator r = GetActorRotation();
			//ABSc3bCharacter* NewPlayer = GetWorld()->SpawnActor<ABSc3bCharacter>(this->GetClass(), t, r);
			//GetController()->Possess(NewPlayer);
			//NewPlayer->Client_Respawn();
		}
		
	}
}

void ABSc3bCharacter::Server_EndHit_Implementation()
{
	bHitByBullet = false;
}

void ABSc3bCharacter::Client_FlipLaserVisibility_Implementation(bool Visible)
{
	//Make sure we only run this on the owning client
	//This is especially import when running on the 
	//Check has to run here, otherwise it does not work correctly
	if (IsLocallyControlled())
	{
		LaserSight->SetVisibility(Visible);
		LaserImpact->SetVisibility(Visible);
	}
	
}

void ABSc3bCharacter::Server_Respawn_Implementation()
{
	Client_ResetInput();
	const FVector t = GetActorLocation();
	const FRotator r = GetActorRotation();
	ABSc3bCharacter* NewPlayer = GetWorld()->SpawnActor<ABSc3bCharacter>(this->GetClass(), t, r);
	GetController()->Possess(NewPlayer);
	//NewPlayer->Client_Respawn();
	NewPlayer->Client_CustomBeginPlay();
}

void ABSc3bCharacter::Client_ResetInput_Implementation()
{
	if (IsValid(PlayerController))
	{
		PlayerController->ShowRespawnButton(false);
	}

}

void ABSc3bCharacter::Client_CustomBeginPlay_Implementation()
{
	if (IsLocallyControlled())
	{
		//Hide the head from our newly spawned player
		FName Head = TEXT("head");
		GetMesh()->HideBoneByName(Head, PBO_None);
	}
}

void ABSc3bCharacter::Client_Respawn_Implementation()
{
	//Any client specific calls needed before respawning
	PlayerController->ShowRespawnButton(true);
}

void ABSc3bCharacter::Server_PlayFootstep_Implementation(FVector Location, USoundBase* Sound, USoundAttenuation* Attenuation)
{
	Multi_PlayFootstep(Location, Sound, Attenuation);
}

void ABSc3bCharacter::Multi_PlayFootstep_Implementation(FVector Location, USoundBase* Sound, USoundAttenuation* Attenuation)
{
	//Error check just in case variables have not been set
	if (IsValid(Sound) && IsValid(Attenuation))
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Sound, Location, FRotator::ZeroRotator, 1, 1, 0, Attenuation);	
	}

}

void ABSc3bCharacter::SpawnBullet(FVector Location, FRotator Rotation)
{
	if (Ammo <= 0)
	{
		return;
	}
	AActor* T = GetWorld()->SpawnActor<AActor>(PlayerController->SpawnObject, Location, Rotation);
	T->SetInstigator(this);
	if (ABullet* Bullet = Cast<ABullet>(T))
	{
		Bullet->AddImpulseToBullet(Weapon->GetRightVector());
	}
	Ammo --;
	//adjust this as it is run on server, do not want this
	if (IsValid(PlayerController->PlayerHUD))
	{
		PlayerController->PlayerHUD->AmmoCount = FString::FromInt(Ammo);	
	}

}

void ABSc3bCharacter::Server_Spawn_Implementation()
{
	Weapon->SpawnAttachment();
}

void ABSc3bCharacter::Move(const FInputActionValue& Value)
{
	// Replicate our move axis vector to be used in player state machine
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (HasAuthority())
	{
		MoveAxisVector = MovementVector;
	} else if (IsLocallyControlled())
	{
		MoveAxisVector = MovementVector;
		Server_PlayerVelocity(MovementVector);
	}

	//Add movement to our player. This value is automatically replicated by our controller
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

		//Sprinting related code
		//Are we sprinting or have we run through this code already?
		if (!bIsSprinting || bStopSprinting)
		{
			return;
		}

		//Are we moving forwards?
		if (MovementVector.Y > 0)
		{
			return;
		}
		
		if (HasAuthority())
		{
			bStopSprinting = true;
			GetCharacterMovement()->MaxWalkSpeed = 200;
		}
		else if (IsLocallyControlled())
		{
			bStopSprinting = true;
			GetCharacterMovement()->MaxWalkSpeed = 200;
			Server_PlayerSprinting(bIsSprinting, 200);
		} 
	} 
}


void ABSc3bCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABSc3bCharacter::Shoot(const FInputActionValue& Value)
{
	//If we are sprinting, do not allow the player to shoot	
	if (bIsSprinting || bIsChangingAttachments)
	{
		return;
	}
	if (Controller != nullptr)
	{
		//Call server function if we currently do no have ROLE_AUTHORITY
		if (!HasAuthority())
		{

			//Change shooting variable for animation. Optimise this into one server call
			bIsShooting = Value.Get<bool>();
			Server_PlayerShooting(Value.Get<bool>());
		} else //meaning we are currently on the server
		{

			bIsShooting = Value.Get<bool>();
		}
		
		
		if (GetOwner()->GetLocalRole() == ROLE_Authority)
		{
			//subtract health on server
			//Health -= 1;
			//OnRep_Health();
		}
		else
		{
			//call server function to subtract health
			//Server_Health();
		}
	}
	
	
}

void ABSc3bCharacter::ShootComplete(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		if (HasAuthority())
		{
			bIsShooting = Value.Get<bool>();
		} else if (IsLocallyControlled())
		{
			//Have to call this on client before going to server otherwise owning client will not
			//see the same thing
			bIsShooting = Value.Get<bool>();
			Server_PlayerShooting(Value.Get<bool>());
		}
	}
	if (bIsPlayerAiming && !LaserSight->IsVisible())
	{
		Client_FlipLaserVisibility(true);
	} else if (!bIsPlayerAiming && LaserSight->IsVisible())
	{
		Client_FlipLaserVisibility(false);
	}
}

void ABSc3bCharacter::Aim(const FInputActionValue& Value)
{
	//If we are sprinting and moving forward, do not allow the player to aim
	float Speed;
	if (bIsSprinting || bIsChangingAttachments || bReloading)
	{
		return;	
	}
	if (!bIsPlayerAiming)
	{
		Speed = 150;
	}
	else
	{
		Speed = 300;
	}
	//Setting our aiming variable to be replicated
	if (HasAuthority())
	{
		bIsPlayerAiming = Value.Get<bool>();
		GetCharacterMovement()->MaxWalkSpeed = Speed;
	} else if (IsLocallyControlled())
	{
		//Have to call this on client before going to server otherwise owning client will not
		//see the same thing
		bIsPlayerAiming = Value.Get<bool>();
		GetCharacterMovement()->MaxWalkSpeed = Speed;
		Server_PlayerAiming(Value.Get<bool>(), Speed);
	}
	
	SpawnClothSound(.3);
	
	if (!bIsShooting)
	{
		//Toggle our laser sight visibility for our client only
		Client_FlipLaserVisibility(Value.Get<bool>());
	}
}

void ABSc3bCharacter::SpawnClothSound(float Duration)
{
	if (!IsValid(PlayerController))
	{
		return;
	}
	if (IsValid(PlayerController->ClothSound))
	{
		//get random float
		float StartTime = UKismetMathLibrary::RandomFloatInRange(0, 26);
		float Pitch = UKismetMathLibrary::RandomFloatInRange(1, 4);
		//play sound at random position
		UAudioComponent* Sound = UGameplayStatics::SpawnSoundAttached(PlayerController->ClothSound, GetMesh(), NAME_None, FVector::Zero(), FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset, false, 1, Pitch, StartTime);
		//stop sound after 1 second
		Sound->StopDelayed(Duration);
	}
}

void ABSc3bCharacter::Sprint(const FInputActionValue& Value)
{
	//We need to check again in case the player stops aiming while holding down the sprint key
	if (bIsPlayerAiming  || bWasAimingCanceled || bIsShooting)
	{
		bWasAimingCanceled = Value.Get<bool>();
		return;
	}
	float Speed;
	//Set speed based on whether we are already sprinting
	if (bIsSprinting)
	{
		Speed = 300;
	}
	else
	{
		Speed = 500;
	}

	//Reset value this is used to optimise related code in Move()
	bStopSprinting = false;
	
	//Adjust walk speed that drives the animation and adjust the boolean that affects
	//other logic inside the player such as shooting
	if (HasAuthority())
	{
		GetCharacterMovement()->MaxWalkSpeed = Speed;
		bIsSprinting = Value.Get<bool>();

	} else if (IsLocallyControlled())
	{
		GetCharacterMovement()->MaxWalkSpeed = Speed;
		Server_PlayerSprinting(Value.Get<bool>(), Speed);	
	}
}

void ABSc3bCharacter::Reload(const FInputActionValue& Value)
{
	if (bIsPlayerAiming)
	{
		return;
	}
	SpawnClothSound(.7);
	if (HasAuthority())
	{
		bReloading = Value.Get<bool>();
	}
	else if (IsLocallyControlled())
	{
		Server_Reload(Value.Get<bool>());
	}
}

void ABSc3bCharacter::OpenAttachments(const FInputActionValue& Value)
{
	if (bIsPlayerAiming)
	{
		return;
	}
	//Show our cursor and stop the mouse from moving our camera
	PlayerController->SetShowMouseCursor(Value.Get<bool>());
	PlayerController->SetIgnoreLookInput(Value.Get<bool>());
	//Set value to stop certain mouse inputs
	bIsChangingAttachments = Value.Get<bool>();
	//Flip our attachment buttons visiblity
	PlayerController->PlayerHUD->SetButtonVisibility(Value.Get<bool>());
	
	//Set our input mode so we can interact with our widget with one click
	if (Value.Get<bool>())
	{
		const FInputModeGameAndUI Input;
		PlayerController->SetInputMode(Input);
	} else
	{
		const FInputModeGameOnly Input;
		PlayerController->SetInputMode(Input);
	}

	SpawnClothSound(.3);
}

void ABSc3bCharacter::ShootLogic(bool bAimingIn)
{
	if (!IsValid(Weapon))
	{
		return;
	}
	//If we are sprinting, do not allow the player to shoot	
	if (Controller != nullptr)
	{
		
		FVector Location;
		FRotator Rotation;
		if (bAimingIn)
		{
			Location = LaserSight->GetComponentLocation();
			Rotation = GetActorRotation();
		}
		else
		{
			const FName MuzzleSocket = TEXT("MuzzleSocket");
			Location = Weapon->GetSocketLocation(MuzzleSocket);
			Rotation = Weapon->GetSocketRotation(MuzzleSocket);
		}
		
		//Call server function if we currently do no have ROLE_AUTHORITY
		 //meaning we are currently on the server
			if (HasAuthority())
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnBullet(Location, Rotation);
				Multi_PlayFootstep(Location, PlayerController->Gunshot, PlayerController->GunshotAttenuation);
			}else
			{
				//UE_LOG(LogTemp, Warning, TEXT("WW"));
				Server_Shoot(Location, Rotation);
			}
		
	}
}

void ABSc3bCharacter::EquipWeaponAttachment(EAttachmentKey Attachment)
{
	Weapon->EquipAttachment(Attachment);
}

void ABSc3bCharacter::Server_EquipWeaponAttachment_Implementation(EAttachmentKey Attachment)
{
	Multicast_EquipWeaponAttachment(Attachment);
}

void ABSc3bCharacter::Multicast_EquipWeaponAttachment_Implementation(EAttachmentKey Attachment)
{
	Weapon->EquipAttachment(Attachment);
}

void ABSc3bCharacter::ToggleMagazineVisibility(bool Hide)
{
	if (Hide)
	{
		Weapon->SpawnMag(TEXT("b_gun_magSocket"));
		Weapon->HideBoneByName(TEXT("b_gun_mag"), EPhysBodyOp::PBO_None);
	} else
	{
		Weapon->UnHideBoneByName(TEXT("b_gun_mag"));
		Weapon->MagActor->Destroy();
	}
	
}

void ABSc3bCharacter::UpdateMagazineTransform()
{
	FTransform SocketT = GetMesh()->GetSocketTransform(TEXT("mag_socket"), RTS_World);
	Weapon->UpdateMagTransform(SocketT);
}

void ABSc3bCharacter::Server_PlayerAiming_Implementation(bool bIsAiming, float speed)
{
	bIsPlayerAiming = bIsAiming;
	GetCharacterMovement()->MaxWalkSpeed = speed;
}

void ABSc3bCharacter::Server_PlayerShooting_Implementation(bool bShooting)
{
	bIsShooting = bShooting;
}

void ABSc3bCharacter::Server_PlayerSprinting_Implementation(bool Sprinting, float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
	bIsSprinting = Sprinting;
}

void ABSc3bCharacter::Server_Reload_Implementation(bool Reload)
{
	bReloading = Reload;
	Ammo = 30;
}

	void ABSc3bCharacter::OrientLaserSight()
{
	// Start and End locations of Laser
	FVector Start = LaserSight->GetComponentLocation();
	FVector End = Start + Weapon->GetRightVector() * LaserDistance;
	//Actors to ignore (which is none)
	const TArray<AActor*> ActorsToIgnore;
	//Store result of linetrace
	FHitResult OutHit;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true);
	//Set end of laser to the first hit object or maximum length
	FName LaserEnd = TEXT("Beam End");
	//Set end of mesh variable inside Niagara system
	LaserSight->SetVectorParameter(LaserEnd, UKismetMathLibrary::SelectVector(OutHit.Location, OutHit.TraceEnd, OutHit.bBlockingHit));
	//Set impact system only when we are hitting an actor
	LaserImpact->SetWorldLocation(OutHit.Location);
}

void ABSc3bCharacter::SetPlayerPitchForOffset()
{
	//Take our float and transform it into a rotator, simultaneously adjusting the float to degrees
	FRotator PlayerRotation = UKismetMathLibrary::MakeRotator(0.0, PlayerPitch, 0.0);
	//Difference between control and player rotation
	FRotator Difference = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation());
	//Interpolate between PlayerPitch and our current pitch value
	FRotator InterpRot = UKismetMathLibrary::RInterpTo(PlayerRotation, Difference, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 10);
	//Clamp the value otherwise we get values of 360
	PlayerPitch = UKismetMathLibrary::ClampAngle(InterpRot.Pitch, -90, 90);
	

	//Update other variables to be sent to the animation class
	PlayerHorizontalVelocity = GetVelocity().Length() * MoveAxisVector.X;
	PlayerVerticalVelocity = GetVelocity().Length() * MoveAxisVector.Y;
	
}

void ABSc3bCharacter::Server_SetPlayerPitchForOffset_Implementation()
{
	SetPlayerPitchForOffset();
}




