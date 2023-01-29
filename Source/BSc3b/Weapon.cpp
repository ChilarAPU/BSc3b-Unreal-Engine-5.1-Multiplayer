// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Attachment.h"

UWeapon::UWeapon()
{
	Attachments.Emplace(RedDot, FAttachmentMesh(Scope, nullptr));
	Attachments.Emplace(LongRange, FAttachmentMesh(Scope, nullptr));
	Attachments.Emplace(Silencer, FAttachmentMesh(Muzzle, nullptr));
	Attachments.Emplace(ForeGrip, FAttachmentMesh(Grip, nullptr));
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
	if (Attachments.Find(Attachment)->Type == Grip)
	{
		if (IsValid(GripActor))
		{
			GripActor->Attachment->SetStaticMesh(Attachments.Find(Attachment)->Mesh);
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

void UWeapon::SpawnAttachment()
{
	FTransform SocketT = GetSocketTransform(TEXT("SightSocket"), RTS_World);
	ScopeActor = GetWorld()->SpawnActor<AAttachment>(AttachmentActor, SocketT.GetLocation(), SocketT.GetRotation().Rotator());
	//Attach attachment to a socket on this weapon
	ScopeActor->GetOwningWeapon(this, TEXT("SightSocket"));

	SocketT = GetSocketTransform(TEXT("MuzzleSocket"), RTS_World);
	MuzzleActor = GetWorld()->SpawnActor<AAttachment>(AttachmentActor, SocketT.GetLocation(), SocketT.GetRotation().Rotator());
	//Attach attachment to a socket on this weapon
	MuzzleActor->GetOwningWeapon(this, TEXT("MuzzleSocket"));

	SocketT = GetSocketTransform(TEXT("GripSocket"), RTS_World);
	GripActor = GetWorld()->SpawnActor<AAttachment>(AttachmentActor, SocketT.GetLocation(), SocketT.GetRotation().Rotator());
	//Attach attachment to a socket on this weapon
	GripActor->GetOwningWeapon(this, TEXT("GripSocket"));
}

void UWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnAttachment();
	
}

void UWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}