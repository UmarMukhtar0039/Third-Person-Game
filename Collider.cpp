// Fill out your copyright notice in the Description page of Project Settings.


#include "Collider.h"
#include "Components/SphereComponent.h"
#include"Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerInput.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ColliderMovementComponent.h"
#include "GameFramework/PlayerController.h"

// Sets default values
ACollider::ACollider()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ROOT"));
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SphereComponent->SetupAttachment(GetRootComponent());
	SphereComponent->InitSphereRadius(40.f);
	//This is collision presets.
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));
	SphereComponent->SetHiddenInGame(true);
	/*bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;*/

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pawn"));
	MeshComponent->SetupAttachment(GetRootComponent());

	//Hard Coding StaticMesh// In text macro we put reference of asset
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	if (MeshAsset.Succeeded())
	{
		MeshComponent->SetStaticMesh(MeshAsset.Object);
		MeshComponent->SetRelativeLocation({ 0.f,0.f,-40.f });
		MeshComponent->SetWorldScale3D(FVector(0.8f));
	}


	springarm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	springarm->SetupAttachment(GetRootComponent());
	springarm->SetRelativeRotation({ -45.f, 0.f, 0.f });
	springarm->TargetArmLength = 400.f;
	springarm->bEnableCameraLag = true;
	springarm->CameraLagSpeed = 3.f;
	//springarm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(springarm,USpringArmComponent::SocketName);
	//Camera->bUsePawnControlRotation = false;
	
	//Settingup Movement
	MovementComponent = CreateDefaultSubobject<UColliderMovementComponent>(TEXT("Movement"));
	MovementComponent->UpdatedComponent = RootComponent;

	CameraInput = FVector2D(0.f, 0.f);
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
	
//	//Here Pawn will move when camera turns
	FRotator newrotation(GetActorRotation());
	newrotation.Yaw += CameraInput.X;
	SetActorRotation(newrotation);

	//to make camera move freely // but pawn local direction will be same therefore 
	//it will not move in camera's direction
	FRotator newspringarmrotation(springarm->GetComponentRotation());
	/*newspringarmrotation.Yaw += CameraInput.X;
	springarm->SetWorldRotation(newspringarmrotation);*/

	//frotator springarmrotation(springarm->getcomponentrotation());
	//setting clamp so that it will not go below -80.f and above -.3.f
	//newspringarmrotation.Yaw += CameraInput.X;
	newspringarmrotation.Pitch = FMath::Clamp(newspringarmrotation.Pitch + CameraInput.Y,-80.f,20.f );
	springarm->SetWorldRotation(newspringarmrotation);
	//UE_LOG(LogTemp, Warning, TEXT("X: %f, Y: %f, Z: %f"), newspringarmrotation.Yaw,newspringarmrotation.Pitch,newspringarmrotation.Roll);
}

// Called to bind functionality to input
void ACollider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("Forward"),this,& ACollider::Forward);
	PlayerInputComponent->BindAxis(TEXT("Right"),this,&ACollider::Right);
	PlayerInputComponent->BindAxis(TEXT("LookUp"),this,&ACollider::PitchCamera);
	PlayerInputComponent->BindAxis(TEXT("Turn"),this, &ACollider::YawCamera);
}

UPawnMovementComponent* ACollider::GetMovementComponent() const
{
	return MovementComponent;
}


void ACollider::Forward(float input)
{		
	FVector Forward = GetActorForwardVector();
	//FVector Forward = springarm->GetForwardVector().GetSafeNormal();
	//AddMovementInput(input * Forward);
	if (MovementComponent)
	{
		MovementComponent->AddInputVector(input * Forward);
	
	}

}

void ACollider::Right(float input)
{
	FVector Right = GetActorRightVector();
	//AddMovementInput(input * Right);
	if (MovementComponent)
	{
		MovementComponent->AddInputVector(input * Right);
	}

}


void ACollider::YawCamera(float axisvalue)
{
	CameraInput.X = axisvalue;
}

void ACollider::PitchCamera(float axisvalue)
{
	CameraInput.Y = axisvalue;
}


//
//void ACollider::AddControllerPitchInput(float Val)
//{
//	if (Val != 0.f && Controller && Controller->IsLocalPlayerController())
//	{
//		APlayerController* const PC = CastChecked<APlayerController>(Controller);
//		//UE_LOG(LogTemp, Warning, TEXT("Pitch Value: %f"), Val);
//		PC->AddPitchInput(Val);
//	}
//
//}
//
//
//
//void ACollider::AddControllerYawInput(float Val)
//{
//	if (Val != 0.f && Controller && Controller->IsLocalPlayerController())
//	{
//		APlayerController* const PC = CastChecked<APlayerController>(Controller);
//		UE_LOG(LogTemp, Warning, TEXT("Yaw Value: %f"), Val);
//		PC->AddYawInput(Val);
//	}
//
//}