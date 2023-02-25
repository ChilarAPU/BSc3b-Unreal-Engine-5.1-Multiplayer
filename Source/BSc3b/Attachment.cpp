// Fill out your copyright notice in the Description page of Project Settings.


#include "Attachment.h"

#include "Weapon.h"

// Sets default values
AAttachment::AAttachment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	Attachment = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon New"));
	Attachment->SetupAttachment(GetRootComponent());
	//Actor does not need any collision as its location is never directly changed from its owner
	Attachment->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UStaticMesh* AAttachment::GetStaticMesh()
{
	return Attachment->GetStaticMesh();
}

void AAttachment::SetStaticMesh(UStaticMesh* Mesh)
{
	Attachment->SetStaticMesh(Mesh);
}

void AAttachment::AttachToWeapon(UWeapon* Weapon, FName Socket)
{
	//Attach this actor to a socket on the owning weapon
	const FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);
	Attachment->AttachToComponent(Weapon, Rules, Socket);
}

// Called when the game starts or when spawned
void AAttachment::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAttachment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

