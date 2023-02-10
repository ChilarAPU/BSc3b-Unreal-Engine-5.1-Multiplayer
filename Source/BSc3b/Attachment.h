// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Attachment.generated.h"

class UWeapon;

UCLASS()
class BSC3B_API AAttachment : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attachment, meta = (AllowPrivateAccess = true))
	USceneComponent* Root;

	/* Weapon that owns this attachment */
	UPROPERTY()
	UWeapon* OwningWeapon;
	
public:	
	// Sets default values for this actor's properties
	AAttachment();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attachment, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* Attachment;

	/* Attach this actor to a socket on the owning weapon */
	UFUNCTION()
	void GetOwningWeapon(UWeapon* Weapon, FName Socket);
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
