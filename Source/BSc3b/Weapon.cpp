// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Attachment.h"

UWeapon::UWeapon()
{
	Attachments.Emplace(RedDot, FAttachmentMesh(Scope, nullptr));
	Attachments.Emplace(LongRange, FAttachmentMesh(Scope, nullptr));
	Attachments.Emplace(Silencer, FAttachmentMesh(Muzzle, nullptr));
}

void UWeapon::EquipAttachment(EAttachmentKey Attachment)
{
	if (Attachments.Find(Attachment)->Type == Scope)
	{
		if (IsValid(ScopeActor))
    	{
    		ScopeActor->Attachment->SetStaticMesh(Attachments.Find(Attachment)->Mesh);
    	}	
	}
	if (Attachments.Find(Attachment)->Type == Muzzle)
	{
		if (IsValid(MuzzleActor))
		{
			MuzzleActor->Attachment->SetStaticMesh(Attachments.Find(Attachment)->Mesh);
		}
	}
	
}

void UWeapon::SpawnMag(FName SocketName)
{
	FTransform SocketT = GetSocketTransform(SocketName, RTS_World);
	MagActor = GetWorld()->SpawnActor<AAttachment>(AttachmentActor, SocketT.GetLocation(), SocketT.GetRotation().Rotator());
	if (IsValid(MagazineMesh))
	{
		MagActor->Attachment->SetStaticMesh(MagazineMesh);
	}
	
}

void UWeapon::UpdateMagTransform(FTransform Transform)
{
	if (IsValid(MagActor))
	{
		MagActor->SetActorTransform(Transform);
	}
	
}

void UWeapon::BeginPlay()
{
	Super::BeginPlay();
	FActorSpawnParameters SpawnParams;
	
	//Spawning sight actor into world
	FTransform SocketT = GetSocketTransform(TEXT("SightSocket"), RTS_World);
	ScopeActor = GetWorld()->SpawnActor<AAttachment>(AttachmentActor, SocketT.GetLocation(), SocketT.GetRotation().Rotator());

	//Spawning muzzle actor into world
	SocketT = GetSocketTransform(TEXT("MuzzleSocket"), RTS_World);
	MuzzleActor = GetWorld()->SpawnActor<AAttachment>(AttachmentActor, SocketT.GetLocation(), SocketT.GetRotation().Rotator());
	
}

void UWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (IsValid(ScopeActor) && IsValid(MuzzleActor))
	{
		FTransform SocketT = GetSocketTransform(TEXT("SightSocket"), RTS_World);
		ScopeActor->SetActorTransform(SocketT);
		
		SocketT = GetSocketTransform(TEXT("MuzzleSocket"), RTS_World);
		MuzzleActor->SetActorTransform(SocketT);
	}

}
