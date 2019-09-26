// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMain::AMain()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; // Camera 600 units far
	CameraBoom->bUsePawnControlRotation = true; //Rotate Arm Based on controller

	//Hard Coding Capsule collision component	
	GetCapsuleComponent()->SetCapsuleSize(48.f,105.f);
	//Making Player Turn and Move in direction where camera is pointing
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.f); //... at this rotation rate i.e. 540.f
	GetCharacterMovement()->JumpZVelocity = 650.f; // Giving velocity of jump
	GetCharacterMovement()->AirControl = 0.2f; // Giving control in air

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//Attach the camera to the end of the boom and let the boom adjust to 
	//match the controller orientation.
	FollowCamera->bUsePawnControlRotation = false;

	//Set our turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	//Don't rotate when the controller rotate 
	//Let it only affect camera.
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

}

// Called when the game starts or when spawned
void AMain::BeginPlay() 
{
	Super::BeginPlay();
	
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//Check if player input component is valid if not it will stop here unless it is valid
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &AMain::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis(TEXT("TurnRate"), this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AMain::LookUpAtRate);
}

void AMain::MoveForward(float value)
{
	if( (Controller!= nullptr) && (value != 0.f) )
	{ // Find the Direction to move when we rotate camera
		const FRotator Rotation = Controller->GetControlRotation();
	//Here Yaw of YawRotation is given by Controller's Yaw.
		const FRotator YawRotation(0.f,Rotation.Yaw,0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction,value);
	}
}

void AMain::MoveRight(float value)
{
		if( (Controller!= nullptr) && (value != 0.f) )
	{ // Find the Direction to move when we rotate camera
			const FRotator Rotation = Controller->GetControlRotation();
	//Here Yaw of YawRotation is given by Controller's Yaw.
		const FRotator YawRotation(0.f,Rotation.Yaw,0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction,value);
	}
}

void AMain::TurnAtRate(float rate)
{
	AddControllerYawInput(rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpAtRate(float rate)
{
	AddControllerPitchInput(rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

