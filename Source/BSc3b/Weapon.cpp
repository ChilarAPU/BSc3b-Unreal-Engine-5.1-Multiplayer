// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Attachment.h"

UWeapon::UWeapon()
{
	Attachments.Emplace(RedDot, FAttachmentMesh(Scope, nullptr));
	Attachments.Emplace(LongRange, FAttachmentMesh(Scope, nullptr));
	Attachments.Emplace(Silencer, FAttachmentMesh(Muzzle, nullptr));
	Attachments.Emplace(ForeGrip, FAttachmentMesh(Grip, nullptr));

	//Default values for weapon statistics
	DamageStat = 5;
	RangeStat = 5.7;
	StabilityStat = 4.3;
	MobilityStat = 4;
}

void UWeapon::SetAttachmentMesh(AAttachment* Actor, EAttachmentKey Attachment, TEnumAsByte<EAttachmentKey>& CachedAttachment)
{
	if (IsValid(Actor))
	{
		if (Actor->Attachment->GetStaticMesh() == Attachments.Find(Attachment)->Mesh)
		{
			//We have clicked the button for the same mesh so do nothing
			return;
		}
		if (CachedAttachment != Attachment)
		{
			//We have clicked a new button of the same attachment type e.g. long scope but have red dot equipped
			//we want to remove the stat changes from our previous attachment before replacing the mesh
			DamageStat -= Attachments.Find(CachedAttachment)->Damage;
			RangeStat -= Attachments.Find(CachedAttachment)->Range;
			StabilityStat -= Attachments.Find(CachedAttachment)->Stability;
			MobilityStat -= Attachments.Find(CachedAttachment)->Mobility;
		}

		//replace mesh and calculate our new stats
		Actor->Attachment->SetStaticMesh(Attachments.Find(Attachment)->Mesh);
		DamageStat += Attachments.Find(Attachment)->Damage;
		RangeStat += Attachments.Find(Attachment)->Range;
		StabilityStat += Attachments.Find(Attachment)->Stability;
		MobilityStat += Attachments.Find(Attachment)->Mobility;
		CachedAttachment = Attachment;
	}
}

void UWeapon::EquipAttachment(EAttachmentKey Attachment)
{
	if (Attachments.Find(Attachment)->Type == Scope)
	{
		SetAttachmentMesh(ScopeActor, Attachment, CachedScopeKey);
	}
	if (Attachments.Find(Attachment)->Type == Muzzle)
	{
		SetAttachmentMesh(MuzzleActor, Attachment, CachedMuzzleKey);
	}
	if (Attachments.Find(Attachment)->Type == Grip)
	{
		SetAttachmentMesh(GripActor, Attachment, CachedGripKey);
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