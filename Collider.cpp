// Fill out your copyright notice in the Description page of Project Settings.

#include "Collider.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ACollider::ACollider()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ROOT"));

	sphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Comp"));
	sphereComponent->SetupAttachment(GetRootComponent());
	sphereComponent->InitSphereRadius(40.f);
	sphereComponent->SetCollisionProfileName(TEXT("Pawn"));

	meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Comp"));
	meshComponent->SetupAttachment(GetRootComponent());
	
	//HardCoding Mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComponentAsset
	(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));

	if ( MeshComponentAsset.Succeeded())
	{
		meshComponent->SetStaticMesh(MeshComponentAsset.Object);
		meshComponent->SetRelativeLocation({ 0.f, 0.f,-40.f });
		meshComponent->SetWorldScale3D(FVector(0.8f, 0.8f, .8f));
	}


	springarmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Component"));
	springarmComponent->SetupAttachment(GetRootComponent());
	springarmComponent->RelativeRotation = FRotator(-45.f, 0.f,0.f);
	springarmComponent->TargetArmLength = 400.f;
	springarmComponent->bEnableCameraLag = true;
	springarmComponent->CameraLagSpeed = 3.f;

	cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	cameraComponent->SetupAttachment(springarmComponent);


	AutoPossessPlayer = EAutoReceiveInput::Player0;

}

// Called when the game starts or when spawned
void ACollider::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void ACollider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACollider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Move Forward"),this,&ACollider::moveForward);
	PlayerInputComponent->BindAxis(TEXT("Move Right"),this,&ACollider::moveRight);
}

void ACollider::moveForward(float input)
{
	FVector Forward = GetActorForwardVector();
	AddMovementInput(input * Forward);
}


void ACollider::moveRight(float input)
{
	FVector Right = GetActorRightVector();
	AddMovementInput(input * Right);
}
