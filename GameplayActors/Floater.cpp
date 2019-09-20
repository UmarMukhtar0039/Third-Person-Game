// Fill out your copyright notice in the Description page of Project Settings.

#include "Floater.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AFloater::AFloater()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	staticmesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("My Mesh"));
	//Init Variables
	initialLocation = FVector(0.0f);
	placedLocation = FVector(0.0f);
	binitable = false;
	WorldOrigin = FVector(0.0f);
	InitialDirection = FVector(0.0f);
	FVector f(0.f);
	bshouldfloat = false;
	Rotate = FRotator(0.f);
	InitialForce = FVector(0.f, 0.f, 0.f);
	InitialTorque = FVector(0.f, 0.f, 0.f);
	RunningTime = 0.f;
	Amplitude = 1.f;
	TimeStretch = 1.f;
}

// Called when the game starts or when spawned
void AFloater::BeginPlay()
{
	Super::BeginPlay();
	float initialX = FMath::FRandRange(-500.f,500.f);
	float initialY = FMath::FRandRange(-500.f, 500.f);
	float initialZ = FMath::FRandRange(0.f, 500.f);
	
	initialLocation.X = initialX;
	initialLocation.Y = initialY;
	initialLocation.Z = initialZ;

	placedLocation = GetActorLocation();

	if(binitable)
	SetActorLocation(initialLocation);

	staticmesh->AddForce(InitialForce);
	staticmesh->AddTorqueInRadians(InitialTorque);
}

// Called every frame
void AFloater::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bshouldfloat)
	{
		//FHitResult HitResult;
		//AddActorLocalOffset( InitialDirection, true , &HitResult);
		//
		//FVector HitLocation = HitResult.Location;

		//UE_LOG(LogTemp, Warning, TEXT("Hit Location: X = %f, Y = %f , Z = %f"),
		//	HitLocation.X, HitLocation.Y, HitLocation.Z);
	
		FVector newLocation = GetActorLocation();

		newLocation.X += Amplitude * FMath::Sin(RunningTime * TimeStretch);
		newLocation.Y += Amplitude * FMath::Cos(RunningTime * TimeStretch);

		SetActorLocation(newLocation);
		RunningTime += DeltaTime;
		
	}

	AddActorLocalRotation(Rotate);
}

