// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BSc3bController.generated.h"

enum EAttachmentKey;
enum EAttachmentType;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

	////// AUDIO VARIABLES //////
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess))
	USoundBase* PlayerHitmarkerSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess))
	USoundBase* HeadshotSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess))
	USoundBase* ClothSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess))
	USoundBase* Gunshot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess))
	USoundBase* EmptyGunshot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess))
	USoundBase* PlayerHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess))
	USoundBase* LaserSightOn;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess))
	USoundBase* LaserSightOff;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess))
	USoundAttenuation* GunshotAttenuation;
	////// END OF AUDIO VARIABLES //////

	/* Used by character to spawn the menu on a button press*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess))
	TSubclassOf<UInGameMenu> InGameMenuClass;
	
	/* Actor which should spawn when the player shoot a weapon */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Bullet, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABullet> SpawnObject;
	
	UPROPERTY()
	UInGameMenu* InGameMenuWidget;

	UPROPERTY()
	UPlayerHUD* PlayerHUD;

	/* Reference to player pawn in the event we need it*/
	UPROPERTY()
	ABSc3bCharacter* PlayerClass;

	/* Reference to the custom game user settings class to access config values*/
	UPROPERTY()
	UCustom_GameUserSettings* UserSettings;

public:
	
	ABSc3bController();

	virtual void BeginPlay() override;

	////// BELOW VARIABLES CALLED AND USED IN ABSC3BCHARACTER //////

	UFUNCTION()
	UPlayerHUD* GetPlayerHUD();

	/* Spawns in game menu with appropriate input settings*/
	UFUNCTION()
	void SpawnInGameMenu();

	/* Returns settings back to default */
	UFUNCTION()
	void RemoveInGameMenu();

	/* Flip the visibility of our respawn button as well as showing/hiding the mouse cursor. This is called upon
	 * player death */
	UFUNCTION()
	void ShowRespawnButton(bool Visible);

	//// AUDIO GETTERS ////
	UFUNCTION()
	USoundBase* GetClothSound();

	UFUNCTION()
	USoundBase* GetGunshotSound();

	UFUNCTION()
	USoundBase* GetEmptyGunshotSound();
	
	UFUNCTION()
	USoundBase* GetPlayerHitSound();

	UFUNCTION()
	USoundBase* GetLaserSightOnSound();

	UFUNCTION()
	USoundBase* GetLaserSightOffSound();

	UFUNCTION()
	USoundAttenuation* GetGunshotAttenuation();

	UFUNCTION()
	TSubclassOf<ABullet> GetBulletClass();

	UFUNCTION()
	UCustom_GameUserSettings* GetConfigUserSettings();

	/*Procedurally interpolate the weapon rotation when the player moves the camera
	* to give the effect of the weapon swaying around. ALl rotation is done in
	* object space so at to avoid any unaccounted negative world space values */
	UFUNCTION()
	void WeaponSway(float DeltaTime, FVector2D LookAxis, UWeapon* Weapon);

	UFUNCTION(Client, Unreliable)
	void Client_ShowHitmarker(FName HitBone);
	void Client_ShowHitmarker_Implementation(FName HitBone);

	/* Used inside of Weapon.h to keep track of the attachment that are on the gun. This is used upon respawn so that
	 * attachments are not lost upon death
	 */
	UPROPERTY()
	TMap<TEnumAsByte<EAttachmentType>,TEnumAsByte<EAttachmentKey>> SavedAttachments;

	UFUNCTION()
	void ShowGameOverMessage(FString WinningPlayerID);

	UFUNCTION(Client, Unreliable)
	void Client_SetEliminationMessage(const FString& OtherPlayersID);
	void Client_SetEliminationMessage_Implementation(const FString& OtherPlayersID);

protected:
	virtual void OnNetCleanup(UNetConnection* Connection) override;
	
};
