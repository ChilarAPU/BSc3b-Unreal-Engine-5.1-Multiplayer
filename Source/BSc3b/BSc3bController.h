// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BSc3bController.generated.h"

class UCustom_GameUserSettings;
class UInGameMenu;
class UPlayerAnimation;
class UPlayerHUD;
class ABSc3bCharacter;
class ABullet;
class UWeapon;
/**
 * 
 */

UCLASS()
class BSC3B_API ABSc3bController : public APlayerController
{
	GENERATED_BODY()
	
	UPROPERTY()
	ABSc3bCharacter* PlayerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UPlayerAnimation> PlayerAnimClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess))
	USoundBase* PlayerHitmarkerSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess))
	USoundBase* HeadshotSound;


public:
	
	ABSc3bController();

	virtual void BeginPlay() override;

	////// BELOW VARIABLES CALLED AND USED IN ABSC3BCHARACTER //////
	UPROPERTY()
	UPlayerHUD* PlayerHUD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UInGameMenu> InGameMenuClass;
	
	UPROPERTY()
	UInGameMenu* InGameMenuWidget;

	/* Encapsulate the TSubclassOf variable into an easier to access and read value */
	UPROPERTY()
	UPlayerAnimation* PlayerAnim;

	/* Flip the visibility of our respawn button as well as showing/hiding the mouse cursor. This is called upon
	 * player death */
	UFUNCTION()
	void ShowRespawnButton(bool Visible);

	////// AUDIO VARIABLES //////
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* ClothSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* Gunshot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* EmptyGunshot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* PlayerHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* LaserSightOn;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* LaserSightOff;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundAttenuation* GunshotAttenuation;

	/* Actor which should spawn when the player shoot a weapon */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Bullet, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABullet> SpawnObject;

	UPROPERTY()
	UCustom_GameUserSettings* UserSettings;

	/*Procedurally interpolate the weapon rotation when the player moves the camera
	* to give the effect of the weapon swaying around. ALl rotation is done in
	* object space so at to avoid any unaccounted negative world space values */
	UFUNCTION()
	void WeaponSway(float DeltaTime, FVector2D LookAxis, UWeapon* Weapon);

	UFUNCTION(Client, Unreliable)
	void Client_ShowHitmarker(FName HitBone);
	void Client_ShowHitmarker_Implementation(FName HitBone);

	/* During load, we set this value from our Epic Online Service GetPlayerUsername() call and allows
	 * each player to be easily identifiable in the kill-feed and scoreboard.
	 */
	UPROPERTY()
	FString PlayerDisplayName;

protected:
	virtual void OnNetCleanup(UNetConnection* Connection) override;
	
};
