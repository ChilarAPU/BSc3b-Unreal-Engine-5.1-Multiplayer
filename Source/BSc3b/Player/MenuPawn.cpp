// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuPawn.h"

#include "BSc3b/UI/MainMenu.h"
#include "Camera/CameraComponent.h"

// Sets default values
AMenuPawn::AMenuPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetRootComponent());  //technically is no root component so Camera becomes it

}

// Called when the game starts or when spawned
void AMenuPawn::BeginPlay()
{
	Super::BeginPlay();
	if (!IsLocallyControlled()) //not really needed as main menu only ever has one client
	{
		return;
	}
	if (!MainMenuWidget)
	{
		return;
	}
	MainMenu = CreateWidget<UMainMenu>(GetWorld(), MainMenuWidget);
	MainMenu->AddToViewport();
		
	APlayerController* CastController = Cast<APlayerController>(GetController());
	if (!Controller)
	{
		return;
	}
	CastController->SetShowMouseCursor(true);
	const FInputModeUIOnly Input;
	CastController->SetInputMode(Input);
}

// Called every frame
void AMenuPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMenuPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

