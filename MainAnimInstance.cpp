// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"


void UMainAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{   // gets the owner of this animation instance
		Pawn = TryGetPawnOwner();
	}
}



void UMainAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
		Pawn = TryGetPawnOwner();

	if (Pawn)
	{
		FVector speed = Pawn->GetVelocity();
		FVector lateralspeed(speed.X, speed.Y, 0.f);
		MovementSpeed = lateralspeed.Size();
		bIsInAir = Pawn->GetMovementComponent()->IsFalling();
	}
		
}

