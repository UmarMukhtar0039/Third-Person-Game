// Fill out your copyright notice in the Description page of Project Settings.


#include "ColliderMovementComponent.h"
#include "Logging/LogMacros.h"

///Can't do this
//UColliderMovementComponent::UColliderMovementComponent()
//	:movespeed(150.f)
//{
//
//}


void UColliderMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Check to make sure everything is still valid
	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	//Get and clear the vector from collider
	FVector DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.f) * DeltaTime * 200.f;
	
	if (!DesiredMovementThisFrame.IsNearlyZero())
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);
		//Sliding along side if we bump into a wall or something.
		if (!Hit.IsValidBlockingHit())
		{
			UE_LOG(LogTemp, Warning, TEXT("hitting target CSDSD"));
			SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
		}
	}
}