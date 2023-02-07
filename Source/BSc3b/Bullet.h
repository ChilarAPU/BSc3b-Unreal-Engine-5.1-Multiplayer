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

	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;
	
public:	
	// Sets default values for this actor's properties
	ABullet();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Bullet")
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Bullet")
	UStaticMeshComponent* BulletMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Particle")
	UNiagaraComponent* BulletTrail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	float BulletSpeed;

	UPROPERTY()
	ABSc3bCharacter* Player;

	void AddImpulseToBullet(FVector Direction);

	UFUNCTION()
	void PlayerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult );
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY()
	FVector CachedLocation;
	
	UFUNCTION()
	void CustomCollision();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
