// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Platform = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floating Platform"));
	Platform->SetupAttachment(RootComponent);

	StartPoint = FVector(0.f);
	EndPoint = FVector(0.f);
	InterpSpeed = 2.f;
	bInterping = false;
	InterpTime = 1.f;

}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	StartPoint = GetActorLocation();
	EndPoint += StartPoint;
	Distance = (EndPoint - StartPoint).Size();
	GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterping)
	{// get current location of the actor and go to the calculated location
		FVector CurrentLocation = GetActorLocation();
		FVector Interp = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, InterpSpeed);
		SetActorLocation(Interp);

		float DistanceTravelled = (GetActorLocation() - StartPoint).Size();
		if (Distance - DistanceTravelled <= 1.f)
		{
			ToggleInterping();
			GetWorldTimerManager().SetTimer(InterpTimer, this,  &AFloatingPlatform::ToggleInterping, InterpTime);
			SwapVector(StartPoint, EndPoint);
		}
	}

}

void AFloatingPlatform::ToggleInterping()
{
	bInterping = !bInterping;
}

void AFloatingPlatform::SwapVector(FVector& vec1, FVector& vec2)
{
	FVector temp = vec1;
	vec1 = vec2;
	vec2 = temp;
}

