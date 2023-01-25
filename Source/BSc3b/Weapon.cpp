// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

UWeapon::UWeapon()
{
	//const FName Socket = TEXT("SightSocket");
	//FVector t = GetSocketLocation(Socket);
	//Scope->SetRelativeLocation(t);
	//Scope->SetupAttachment(GetAttachmentRoot(), Socket);

	Attachments.Emplace(None, nullptr);
	Attachments.Emplace(RedDot, nullptr);
	Attachments.Emplace(LongRange, nullptr);
}

void UWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}
