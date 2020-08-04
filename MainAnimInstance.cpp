// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Main.h"

void UMainAnimInstance::NativeInitializeAnimation()
{	
	if (Pawn == nullptr)
	{   // gets the owner of this animation instance
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Main = Cast<AMain>(Pawn);
		}
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
		//UE_LOG(LogTemp, Warning, TEXT("Movement Speed: %f"), MovementSpeed);
		
		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		if (Main == nullptr)
		{
			Main = Cast<AMain>(Pawn);
		}
	}
		
}

