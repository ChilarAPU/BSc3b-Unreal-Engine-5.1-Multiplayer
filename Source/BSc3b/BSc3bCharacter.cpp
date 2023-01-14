// Copyright Epic Games, Inc. All Rights Reserved.

#include "BSc3bCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
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
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a follow camera
	const FName CameraSocketName = TEXT("cameraSocket");
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(GetMesh(), CameraSocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm
	FollowCamera->SetRelativeLocation(FVector(-4.506538, 0.355924, -4.909522)); //Set Location of camera
	//Dont need to set rotation as the camera is attached to the head socket of mesh, overriding any rotation values

	//Create weapon static mesh component
	const FName WeaponSocketName = TEXT("weapon_handle");
	Weapon1 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon1->SetupAttachment(GetMesh(), WeaponSocketName);

	const FName LaserSocketName = TEXT("b_gun_muzzleflash");
	LaserSight = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LaserSight"));
	LaserSight->SetupAttachment(Weapon1, LaserSocketName);
	LaserSight->SetVisibility(false);
	LaserSight->SetIsReplicated(false);

	LaserImpact = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LaserImpact"));
	LaserImpact->SetupAttachment(Weapon1);
	LaserImpact->SetVisibility(false);
	LaserImpact->SetIsReplicated(false);

	//Will Setup custom movement rotation as this rotates the mesh for any movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true;
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	LaserDistance = 1000;
	
	Health = 100;
	OnRep_Health();
	PlayerPitch = 0;
	PlayerHorizontalVelocity = 0;
	PlayerVerticalVelocity = 0;
	bIsPlayerAiming = false;
	
}

void ABSc3bCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
		//set the visibility of the laser sight to true only for the client
		//LaserSight->SetVisibility(true);
		//LaserImpact->SetVisibility(true);
	}
}

void ABSc3bCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	OrientLaserSight();
	WeaponSway(DeltaSeconds);
	//Aim Offset replication code
	if (HasAuthority())
	{
		SetPlayerPitchForOffset();
	} else if (IsLocallyControlled())
	{
		SetPlayerPitchForOffset();
		Server_SetPlayerPitchForOffset();
	}
}

void ABSc3bCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Tell it to replicate the variable all clients at all times
	DOREPLIFETIME(ABSc3bCharacter, Health);
	
	//Replicated variables used in animations
	DOREPLIFETIME_CONDITION(ABSc3bCharacter, PlayerHorizontalVelocity, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABSc3bCharacter, PlayerVerticalVelocity, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABSc3bCharacter, bIsPlayerAiming, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABSc3bCharacter, MoveAxisVector, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABSc3bCharacter, PlayerPitch, COND_SkipOwner);
}

void ABSc3bCharacter::OnRep_Health()
{
	
}

FTransform ABSc3bCharacter::GetWeaponTransform(FName Socket, ERelativeTransformSpace TransformSpace)
{
	return Weapon1->GetSocketTransform(Socket, TransformSpace);
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
	UE_LOG(LogTemp, Warning, TEXT("Server_Shoot_Implementation HAS BEEN CALLED"));
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnBullet(Location, Rotation);
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
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABSc3bCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABSc3bCharacter::Look);

		//Shooting
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ABSc3bCharacter::Shoot);

		//Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ABSc3bCharacter::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ABSc3bCharacter::Aim);

	}

}

bool ABSc3bCharacter::Server_Health_Validate()
{
	return true;
}

void ABSc3bCharacter::Server_Health_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		Health -= 1;
		
		//temporary way to spawn in new player character
		const FVector t = GetActorLocation();
		const FRotator r = GetActorRotation();
		ABSc3bCharacter* NewPlayer = GetWorld()->SpawnActor<ABSc3bCharacter>(this->GetClass(), t, r);
		GetController()->Possess(NewPlayer);
		//Call client function on new player actor. This sets the laser sight up so that other clients cannot see it
		//NewPlayer->Client_Respawn();
	}

	//OnRep_Health();
}

void ABSc3bCharacter::Client_FlipLaserVisibility_Implementation()
{
	//Make sure we only run this on the owning client
	//This is especially import when running on the 
	//Check has to run here, otherwise it does not work correctly
	if (IsLocallyControlled())
	{
		LaserSight->ToggleVisibility();
		LaserImpact->ToggleVisibility();
	}
	
}

void ABSc3bCharacter::Server_PlayFootstep_Implementation(FVector Location)
{
	Multi_PlayFootstep(Location);
}

void ABSc3bCharacter::Multi_PlayFootstep_Implementation(FVector Location)
{
	//Error check just in case variables have not been set
	if (IsValid(Footstep) && IsValid(FootstepAttenuation))
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Footstep, Location, FRotator::ZeroRotator, 1, 1, 0, FootstepAttenuation);	
	}

}

void ABSc3bCharacter::SpawnBullet(FVector Location, FRotator Rotation)
{
	AActor* T = GetWorld()->SpawnActor<AActor>(SpawnObject, Location, Rotation);
	T->SetInstigator(this);
	if (ABullet* Bullet = Cast<ABullet>(T))
	{
		Bullet->AddImpulseToBullet(Weapon1->GetRightVector());
	}
}

void ABSc3bCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (HasAuthority())
	{
		MoveAxisVector = MovementVector;
	} else if (IsLocallyControlled())
	{
		MoveAxisVector = MovementVector;
		Server_PlayerVelocity(MovementVector);
	}
	
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
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
	if (Controller != nullptr)
	{
		
		if (GetOwner()->GetLocalRole() == ROLE_Authority) //Only server has the role of authority
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, FString::Printf(TEXT("%s: Shoot"), *GetOwner()->GetRolePropertyName().ToString()));
		} 
		/*if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy) //Client With Human based movement has this role
			{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, FString::Printf(TEXT("%s: Shoot"), *GetRolePropertyName().ToString()));
			}*/
		const FName MuzzleSocket = TEXT("MuzzleSocket");
		const FVector Location = LaserSight->GetComponentLocation();;
		const FRotator Rotation = GetActorRotation();
		
		//Call server function if we currently do no have ROLE_AUTHORITY
		if (!HasAuthority())
		{
			Server_Shoot(Location, Rotation);
		} else //meaning we are currently on the server
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnBullet(Location, Rotation);
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

void ABSc3bCharacter::Aim(const FInputActionValue& Value)
{
	//Setting our aiming variable to be replicated
	if (HasAuthority())
	{
		bIsPlayerAiming = Value.Get<bool>();
	} else if (IsLocallyControlled())
	{
		//Have to call this on client before going to server otherwise owning client will not
		//see the same thing
		bIsPlayerAiming = Value.Get<bool>();
		Server_PlayerAiming(Value.Get<bool>());
	}
	Client_FlipLaserVisibility();
	if (IsValid(AimSound))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), AimSound);
	}
}

void ABSc3bCharacter::Server_PlayerAiming_Implementation(bool bIsAiming)
{
	bIsPlayerAiming = bIsAiming;
}

void ABSc3bCharacter::OrientLaserSight()
{
	// Start and End locations of Laser
	FVector Start = LaserSight->GetComponentLocation();
	FVector End = Start + Weapon1->GetRightVector() * LaserDistance;
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

void ABSc3bCharacter::WeaponSway(float DeltaTime)
{
	//Values the gun will be interpolating between
	FRotator FinalRot, InitRot;
	//Maximum value the gun can sway
	const float MaxSwayDegree = 2.5;
	//Speed at which the gun reaches the final rotation value
	const float InterpSpeed = 2.5;
	//Multiplying each lookaxis vector value by -1 flips the incoming direction
	//LookAxisVector relates to the speed of our mouse movement
	FinalRot = FRotator(LookAxisVector.Y * -1 * MaxSwayDegree, LookAxisVector.X * -1 * MaxSwayDegree, LookAxisVector.X * -1 * MaxSwayDegree);
	FRotator RotationDifference = UKismetMathLibrary::MakeRotator(InitRot.Roll + FinalRot.Roll, InitRot.Pitch - FinalRot.Pitch, InitRot.Yaw + FinalRot.Yaw);
	//Do all our interping in relative space as this keeps our values to normal amounts
	FRotator InterpDifference = UKismetMathLibrary::RInterpTo(Weapon1->GetRelativeRotation(), RotationDifference, DeltaTime, InterpSpeed);
	//Variables to hold the break rotator function call
	float Roll, Pitch, Yaw;
	UKismetMathLibrary::BreakRotator(InterpDifference, Roll, Pitch, Yaw);
	//Clamp all values between MaxSwayDegree and MaxSwayDegree * -1 otherwise our gun would not stop swaying
	InterpDifference = UKismetMathLibrary::MakeRotator(UKismetMathLibrary::FClamp(Roll, MaxSwayDegree * -1, MaxSwayDegree),
		UKismetMathLibrary::FClamp(Pitch, MaxSwayDegree * -1, MaxSwayDegree), UKismetMathLibrary::FClamp(Yaw, MaxSwayDegree * -1, MaxSwayDegree));
	Weapon1->SetRelativeRotation(InterpDifference);
}

void ABSc3bCharacter::Server_SetPlayerPitchForOffset_Implementation()
{
	SetPlayerPitchForOffset();
}




