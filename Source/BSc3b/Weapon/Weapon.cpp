// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "../Player/BSc3bCharacter.h"
#include "Attachment.h"

UWeapon::UWeapon()
{
	//Default values to fill out the TMap in blueprint
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

void UWeapon::SpawnAttachmentLogic(FName Socket, AAttachment*& ActorAttachment)
{
	//Spawn sight actor at the transform of the socket
	FTransform SocketTransform = GetSocketTransform(Socket, RTS_World);
	ActorAttachment = GetWorld()->SpawnActor<AAttachment>(AttachmentActor, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator());
	//Attach attachment to a socket on this weapon
	ActorAttachment->AttachToWeapon(this, Socket);
}

void UWeapon::SetAttachmentMesh(AAttachment* Actor, EAttachmentKey Attachment, TEnumAsByte<EAttachmentKey>& CachedAttachment)
{
	if (!IsValid(Actor))
	{
		return;
	}
	
	if (Actor->GetStaticMesh() == Attachments.Find(Attachment)->Mesh)    //Is the incoming mesh already on the weapon
	{
		return;
	}
	if (CachedAttachment)   //Is there an attachment already on the gun in the socket
	{
		if (CachedAttachment != Attachment)  //Do not remove the statistics of our new attachment
			{
			//Remove the stat changes from our previous attachment before replacing the mesh
			DamageStat -= Attachments.Find(CachedAttachment)->Damage;
			RangeStat -= Attachments.Find(CachedAttachment)->Range;
			StabilityStat -= Attachments.Find(CachedAttachment)->Stability;
			MobilityStat -= Attachments.Find(CachedAttachment)->Mobility;
			}	
	}
		

	//replace mesh and calculate our new stats
	Actor->SetStaticMesh(Attachments.Find(Attachment)->Mesh);
	
	DamageStat += Attachments.Find(Attachment)->Damage;
	RangeStat += Attachments.Find(Attachment)->Range;
	StabilityStat += Attachments.Find(Attachment)->Stability;
	MobilityStat += Attachments.Find(Attachment)->Mobility;
	
	//Cache our attachment so we can remove the stats on the next attachment change
	CachedAttachment = Attachment;
}

void UWeapon::EquipAttachment(EAttachmentKey Attachment)
{
	// Set the static mesh inside of the correct socket
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

void UWeapon::DestroyMagActor()
{
	MagActor->Destroy();
}

void UWeapon::SpawnMag(FName SocketName, FName SocketAttachName, ABSc3bCharacter* Player)
{
	//Spawns magazine actor at our associate left hand socket. Called by reload notify event
	FTransform SocketT = GetSocketTransform(SocketName, RTS_World);
	MagActor = GetWorld()->SpawnActor<AAttachment>(AttachmentActor, SocketT.GetLocation(), SocketT.GetRotation().Rotator());
	if (IsValid(MagazineMesh))
	{
		//Set the mesh of the magazine and attach it to the passed through bone/socket
		MagActor->SetStaticMesh(MagazineMesh);
		const FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);
		MagActor->AttachToActor(Player, Rules, SocketAttachName);
	}
	
}

void UWeapon::SpawnAttachment()
{
	//Spawn actors for all available sockets
	SpawnAttachmentLogic(TEXT("SightSocket"), ScopeActor);
	SpawnAttachmentLogic(TEXT("MuzzleSocket"), MuzzleActor);
	SpawnAttachmentLogic(TEXT("GripSocket"), GripActor);
}

void UWeapon::DestroyAttachments()
{
	ScopeActor->Destroy();
	MuzzleActor->Destroy();
	GripActor->Destroy();
}

void UWeapon::BeginPlay()
{
	Super::BeginPlay();

	//Spawn all attachment actors at the beginning with empty static mesh values
	SpawnAttachment();
	
}

void UWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}