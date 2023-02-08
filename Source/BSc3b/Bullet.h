// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

class UNiagaraComponent;
//Forward Declarations
class ABSc3bCharacter;

UCLASS()
class BSC3B_API ABullet : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Bullet", meta = (AllowPrivateAccess))
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Particle",  meta = (AllowPrivateAccess))
	UNiagaraComponent* BulletTrail;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Bullet", meta = (AllowPrivateAccess))
	UStaticMeshComponent* BulletMesh;

	/* Used in line trace to specify what actors it should ignore */
	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;

	/* Reference to the player that shot this bullet */
	UPROPERTY()
	ABSc3bCharacter* Player;

	/* Holds the location of the Bullet from the previous frame, This is used to
	 * determine where to spawn the linetrace
	 */
	UPROPERTY()
	FVector CachedLocation;

	/* Custom collision that overrides Unreal's default collision handling. Is designed to work
	 * with smaller objects
	 */
	UFUNCTION()
	void CustomCollision();
	
public:	
	// Sets default values for this actor's properties
	ABullet();

	/* Determines how strong of an impulse should be applied. This gets adjusted along with the range statistic of
	 * a weapon
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	float BulletSpeed;

	/* Add an impulse to our bullet given a specified direction vector*/
	void AddImpulseToBullet(FVector Direction);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
