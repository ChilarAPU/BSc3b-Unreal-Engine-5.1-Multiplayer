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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attachment, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* Attachment;
	
public:	
	// Sets default values for this actor's properties
	AAttachment();

	/* Get the visual mesh of the attachment*/
	UFUNCTION()
	UStaticMesh* GetStaticMesh();

	/* Set the visual mesh of the attachment*/
	UFUNCTION()
	void SetStaticMesh(UStaticMesh* Mesh);

	/* Attach this actor to a socket on the owning weapon */
	UFUNCTION()
	void AttachToWeapon(UWeapon* Weapon, FName Socket);
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
