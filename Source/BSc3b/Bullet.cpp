// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

#include "BSc3bCharacter.h"
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
	BulletMesh->SetNotifyRigidBodyCollision(true);

	BulletTrail = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Trail"));
	BulletTrail->SetupAttachment(BulletMesh);

	//BulletMesh->OnComponentBeginOverlap.AddDynamic(this, &ABullet::PlayerOverlap);

	//Set bullet to despawn 10 seconds after it has been fired. Quick and easy way to stop
	//our level from lagging due to too many meshes
	InitialLifeSpan = 10;

	BulletSpeed = 1000;
	bReplicates = true;
	BulletMesh->SetSimulatePhysics(true);
	Player = nullptr;
	
}

void ABullet::AddImpulseToBullet(FVector Direction)
{
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
			//HitPlayer->Server_Health();
		}
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

void ABullet::CustomCollision()
{
	FVector End = BulletMesh->GetComponentLocation();
	const TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), CachedLocation, End, TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::Persistent, OutHit, true);
	if (OutHit.bBlockingHit)
	{
		ABSc3bCharacter* HitPlayer = Cast<ABSc3bCharacter>(OutHit.GetActor());
		FName HitBone =  OutHit.BoneName;
		if (HitPlayer)
		{
			if (HitPlayer->IsPlayerControlled())
			{
				HitPlayer->Server_Health(HitBone);
			}
			
		}
	}
	
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
	//have to set this in begin play as the constructor does not work
	BulletMesh->OnComponentHit.AddDynamic(this, &ABullet::OnHit);
	
	Player = Cast<ABSc3bCharacter>(GetInstigator());
	CachedLocation = GetActorLocation();
	
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (HasAuthority())
	{
		CustomCollision();
		FVector t = BulletMesh->GetComponentLocation();
		//Storing our current location, ready for the next tick
		CachedLocation = BulletMesh->GetComponentLocation();
	}

}

