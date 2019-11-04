// Fill out your copyright notice in the Description page of Project Settings.


#include "Critter.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"

// Sets default values
ACritter::ACritter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//Create Default SceneCoponent as root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ROOT"));
	StaticMesh = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("Mesh"));
	//Attaching staticmesh to rootcomponent
	StaticMesh->SetupAttachment(GetRootComponent());


	springarm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Springarm"));
	springarm->SetupAttachment(GetRootComponent());

	//Setting up camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	//Attaching camera to root
	Camera->SetupAttachment(springarm);
	Camera->SetRelativeLocation(FVector(-300.f, 0.f, 0.f), true);
	Camera->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	
	CurrentVelocity = FVector(0.f);
	speed = 50.f;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
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
	//CurrentVelocity = CurrentVelocity.GetSafeNormal();
	FVector Newlocation(GetActorLocation() + CurrentVelocity * DeltaTime );
	//UE_LOG(LogTemp, Warning, TEXT("X : %f , Y : %f"), CurrentVelocity.X, CurrentVelocity.Y);
	FHitResult hit;
	SetActorLocation(Newlocation,true,&hit,ETeleportType::TeleportPhysics);
}

// Called to bind functionality to input
void ACritter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("Forward"),this,&ACritter::Forward);
	PlayerInputComponent->BindAxis(TEXT("Right"),this,&ACritter::Right);
}


void ACritter::Forward(float input)
{
	CurrentVelocity.X = FMath::Clamp(input, -1.f, 1.f) * speed;
}

void ACritter::Right(float input)
{
	CurrentVelocity.Y = FMath::Clamp(input, -1.f, 1.f) * speed;
}

