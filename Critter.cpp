// Fill out your copyright notice in the Description page of Project Settings.

#include "Critter.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"

// Sets default values
ACritter::ACritter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Custom_Mesh"));

	meshComponent->SetupAttachment(GetRootComponent());

	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	camera->SetupAttachment(GetRootComponent());
	camera->SetRelativeLocation(FVector(300.f,0.f,300.f));
	camera->SetRelativeRotation(FRotator(-30.f,0.f,0.f));

	// Set Default player to be 0
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	currentVelocity = FVector(0.f);
	maxSpeed = 100.f;
}

// Called when the game starts or when spawned
void ACritter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACritter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector NewLocation = GetActorLocation() + currentVelocity * DeltaTime;
	SetActorLocation(NewLocation);

}

// Called to bind functionality to input
void ACritter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Move Forward"), this, &ACritter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Move Right"), this, &ACritter::MoveRight);
}

void ACritter::MoveForward(float value)
{
	currentVelocity.X = FMath::Clamp(value, -1.f, 1.f) * maxSpeed;
}

void ACritter::MoveRight(float value)
{
	currentVelocity.Y = FMath::Clamp(value, -1.f, 1.f) * maxSpeed;
}


