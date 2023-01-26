// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Attachment.h"

UWeapon::UWeapon()
{
	Attachments.Emplace(RedDot, FAttachmentMesh(Scope, nullptr));
	Attachments.Emplace(LongRange, FAttachmentMesh(Scope, nullptr));
	//Attachments.Emplace(RedDot, nullptr);
	//Attachments.Emplace(LongRange, nullptr);
}

void UWeapon::EquipAttachment(EAttachmentKey Attachment)
{
	if (IsValid(ScopeActor))
	{
		ScopeActor->Attachment->SetStaticMesh(Attachments.Find(Attachment)->Mesh);
		UE_LOG(LogTemp, Warning, TEXT("Attachment"));
	}
}

void UWeapon::BeginPlay()
{
	Super::BeginPlay();
	FActorSpawnParameters SpawnParams;
	FTransform SocketT = GetSocketTransform(TEXT("SightSocket"), RTS_World);
	ScopeActor = GetWorld()->SpawnActor<AAttachment>(AttachmentActor, SocketT.GetLocation(), SocketT.GetRotation().Rotator());
	
	
}

void UWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	FTransform SocketT = GetSocketTransform(TEXT("SightSocket"), RTS_World);
	if (IsValid(ScopeActor))
	{
		ScopeActor->SetActorTransform(SocketT);
	}

}
