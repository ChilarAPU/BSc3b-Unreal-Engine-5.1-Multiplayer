// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

#include "BSc3bCharacter.h"

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
	BulletMesh->SetRelativeRotation(FRotator(0.0, -90, 0.0));
	BulletMesh->SetIsReplicated(true);
	BulletMesh->SetNotifyRigidBodyCollision(true);

	BulletMesh->OnComponentBeginOverlap.AddDynamic(this, &ABullet::PlayerOverlap);

	BulletSpeed = 1000;
	bReplicates = true;
	BulletMesh->SetSimulatePhysics(true);
	Player = nullptr;
	
}

void ABullet::AddImpulseToBullet(FVector Direction)
{
	//if using same mesh as before, this is required as the mesh is orientated weirdly
	/*if (IsValid(Player))
	{
		const FVector BForward = Player->GetWeaponForwardVector();
    	BulletMesh->AddImpulse(BForward * BulletSpeed, NAME_None);	
	}
	*/
	const FVector BulletForward = BulletMesh->GetRightVector();
	BulletMesh->AddImpulse(Direction * BulletSpeed, NAME_None);	
	
}

void ABullet::PlayerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABSc3bCharacter* HitPlayer = Cast<ABSc3bCharacter>(OtherActor);
	if (HitPlayer)
	{
		//Without this, clients would not be able to call their server RPCs
		if (HitPlayer->IsLocallyControlled())
		{
			HitPlayer->Server_Health();
		}
		//temporary setting to stop bullets from dealing damage 
		//BulletMesh->SetGenerateOverlapEvents(false);
	}
	
}

void ABullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse, const FHitResult& Hit)
{
	//We have hit a solid object, stop overlap events to avoid accidental overlap events
	UE_LOG(LogTemp, Warning, TEXT("Bullet Hit Event"));
	BulletMesh->SetGenerateOverlapEvents(false);
	//Stop hit events from being constantly outputted as we have no more need for them
	BulletMesh->SetNotifyRigidBodyCollision(false);
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
	//have to set this in begin play as the constructor does not work
	BulletMesh->OnComponentHit.AddDynamic(this, &ABullet::OnHit);
	
	Player = Cast<ABSc3bCharacter>(GetInstigator());
	if (IsValid(Player))
	{
		if (!HasAuthority())
		{
			//UE_LOG(LogTemp, Warning, TEXT("CLIENT"));
		} else //meaning we are currently on the server
			{
			//UE_LOG(LogTemp, Warning, TEXT("SERVER"));
			}
	}
	//AddImpulseToBullet();
	
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

