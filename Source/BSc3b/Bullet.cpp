// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

#include "BSc3bCharacter.h"
#include "Kismet/KismetMathLibrary.h"
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
	//Will not block the players movement
	BulletMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BulletMesh->SetRelativeRotation(FRotator(0.0, -90, -90));
	BulletMesh->SetRelativeScale3D(FVector(0.1, 0.1, 0.1));
	BulletMesh->SetIsReplicated(true);

	BulletTrail = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Trail"));
	BulletTrail->SetupAttachment(BulletMesh);

	//Set bullet to despawn 10 seconds after it has been fired. Quick and easy way to stop
	//our level from lagging due to too many meshes
	InitialLifeSpan = 10;

	BulletSpeed = 1000;
	bReplicates = true;
	BulletMesh->SetSimulatePhysics(true);
	Player = nullptr;
	BulletMesh->SetNotifyRigidBodyCollision(false);
	
}

void ABullet::AddImpulseToBullet(FVector Direction)
{
	BulletMesh->AddImpulse(Direction * BulletSpeed, NAME_None);
}

void ABullet::CustomCollision()
{
	FVector End = BulletMesh->GetComponentLocation();
	FHitResult OutHit;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), CachedLocation, End, TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Persistent, OutHit, true);
	if (OutHit.bBlockingHit)
	{
		ABSc3bCharacter* HitPlayer = Cast<ABSc3bCharacter>(OutHit.GetActor());
		FName HitBone =  OutHit.BoneName;
		//If we have hit a player
		if (HitPlayer)
		{
			//make sure we run this locally and only on the hit player
			if (HitPlayer->IsPlayerControlled())
			{
				HitPlayer->Server_Health(HitBone);
				ActorsToIgnore.Emplace(HitPlayer);
				return;
			}
		}
		//If we have overlapped with an object other than a player
		BulletMesh->SetGenerateOverlapEvents(false);
		//Stop hit events from being constantly outputted as we have no more need for them
		BulletMesh->SetNotifyRigidBodyCollision(false);
		BulletMesh->SetSimulatePhysics(false);
	}
	
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
	
	Player = Cast<ABSc3bCharacter>(GetInstigator());
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
		CustomCollision();
		//Storing our current location, ready for the next tick
		CachedLocation = BulletMesh->GetComponentLocation();
	}

}

