// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

#include "../Player/BSc3bCharacter.h"
#include "../Player/BSc3bController.h"
#include "BSc3b/Player/MenuGameState.h"
#include "BSc3b/Player/PlayerStatistics.h"
#include "BSc3b/UI/PlayerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(DefaultSceneRoot);
	
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetupAttachment(GetRootComponent());
	//Set this mesh to ignore our line trace channel that is used for the laser sight
	BulletMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	BulletMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	//Do not want this bullet to block the collision channel of a player
	BulletMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	BulletMesh->SetRelativeRotation(FRotator(0.0, -90, -90));
	BulletMesh->SetRelativeScale3D(FVector(0.1, 0.1, 0.1));
	//Do not need this bullet to have its movement perfectly replicated among clients as the clients mesh are only visual
	BulletMesh->SetIsReplicated(true);

	BulletTrail = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Trail"));
	BulletTrail->SetupAttachment(BulletMesh);

	//Should never get to the point of being alive for 10 seconds but in the event something goes wrong, we can
	//automatically remove it
	InitialLifeSpan = 10;

	//Decides how far the bullet should travel
	BulletSpeed = 1000;
	
	//Just in case SetIsReplicated() gets skipped
	bReplicates = true;
	
	BulletMesh->SetSimulatePhysics(true);
	Player = nullptr;
	//Used for bullet network size optimization
	ShouldWeCheckCollision = 0;
	
	//Do not want to listen for any hit events on the mesh as we use a line trace for collision instead
	BulletMesh->SetNotifyRigidBodyCollision(false);
	
	/*NOTE: Most collision calls here are likely unnecessary but are kept here just in case changing one setting
	 *does not automatically result in the other ones following */
	
}

void ABullet::AddImpulseToBullet(FVector Direction)
{
	// BulletSpeed is the only variable that should get changed here 
	BulletMesh->AddImpulse(Direction * BulletSpeed, NAME_None);
}

void ABullet::CustomCollision()
{
	//Start of line trace is our cached location
	//End of line trace is our bullets currently location
	FVector End = BulletMesh->GetComponentLocation();
	FHitResult OutHit;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), CachedLocation, End, TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true);
	if (OutHit.bBlockingHit)
	{
		ABSc3bCharacter* HitPlayer = Cast<ABSc3bCharacter>(OutHit.GetActor());
		FName HitBone =  OutHit.BoneName;
		//If we have hit a player
		if (HitPlayer)
		{
			Player = Cast<ABSc3bCharacter>(GetInstigator());
			if (Player->IsPlayerControlled())  //Logic for the player that shot the bullet
			{
				Player->GetActivePlayerController()->Client_ShowHitmarker(HitBone);
			}
			
			if (HitPlayer->IsPlayerControlled()) //Logic for the player that got hit
			{
				//pass through hit bone to adjust damage based on the bone hit
				HitPlayer->Server_Health(HitBone, HitPlayer->GetPlayerOnlineName(), Player->GetPlayerOnlineName());
				//Scoreboard relates changes here. Done here as we need access to both the owner and the player being killed
				if (HitPlayer->Health <= 0)
				{
					AdjustPlayerStateValues(HitPlayer, Player);
				}
				//Play client specific functionality from getting hit
				HitPlayer->Client_PlayHit();
				//Add the hit actor to our ignore hit so we cannot hit the same player twice
				ActorsToIgnore.Emplace(HitPlayer);
				return;
			}
		}
		//If we have overlapped with an object other than a player
		BulletMesh->SetGenerateOverlapEvents(false);
		//Stop hit events from being constantly outputted as we have no more need for them
		BulletMesh->SetNotifyRigidBodyCollision(false);
		//Stop our physics from simulating to save performance
		BulletMesh->SetSimulatePhysics(false);
		//Spawn particle system to visualize to the player where the bullet has landed
		SpawnWallParticle(OutHit.Location);
		//Destroy the bullet
		K2_DestroyActor();
		
		/*UGameplayStatics::SpawnDecalAttached(BulletHoleDecal, FVector(.5, .5, .5), OutHit.GetComponent(), NAME_None,
			OutHit.Location, OutHit.Normal.Rotation(), EAttachLocation::KeepWorldPosition, 10); */
	}
	
}

void ABullet::AdjustPlayerStateValues(ABSc3bCharacter* HitPlayer, ABSc3bCharacter* OwnerOfBullet)
{
	//Get a reference to both relevant player state classes
	APlayerStatistics* HitPlayerStatistics = Cast<APlayerStatistics>(HitPlayer->GetPlayerState());
	APlayerStatistics* OwnerPlayerStatistics = Cast<APlayerStatistics>(OwnerOfBullet->GetPlayerState());
	if (HitPlayerStatistics && OwnerPlayerStatistics)
	{
		HitPlayerStatistics->AddDeathToScore(1);
		OwnerPlayerStatistics->AddKillToScore(1);
	}
	if (OwnerOfBullet->IsPlayerControlled())
	{
		OwnerOfBullet->GetActivePlayerController()->Client_SetEliminationMessage(HitPlayer->GetPlayerOnlineName());
	}
}

void ABullet::SpawnWallParticle_Implementation(FVector Location)
{
	
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	//called again as Unreal likes to sometimes skip the function in our constructor
	SetReplicateMovement(true);
	
	Player = Cast<ABSc3bCharacter>(GetInstigator());
	//Used for the start location of our line trace
	CachedLocation = GetActorLocation();
	//Make sure we cannot hit the player that shot this bullet
	ActorsToIgnore.Emplace(Player);
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (HasAuthority())
	{
		/*if (ShouldWeCheckCollision > 3)
		{
			CustomCollision();
			//Storing our current location, ready for the next tick
			CachedLocation = BulletMesh->GetComponentLocation();
			ShouldWeCheckCollision = 0;
		}
		*/
		CustomCollision();
		//Storing our current location, ready for the next tick
		CachedLocation = BulletMesh->GetComponentLocation();
		
	}

}

