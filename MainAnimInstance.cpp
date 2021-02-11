// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Main.h"
#include "Kismet/GameplayStatics.h"


void UMainAnimInstance::NativeInitializeAnimation()
{	
	if (Pawn == nullptr)
	{   // gets the owner of this animation instance
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Main = Cast<AMain>(Pawn);

			NeckRotation = FRotator(0.f);
			bShouldRotateHead = false;
			if (Main)
			{
				Mesh = Main->GetMesh();
			}
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

		// Character can rotate head only when it is standing idle, or has a 0 velocity.s
		if (Main->GetMovementComponent()->Velocity.Size() == 0)
		{	
			bShouldRotateHead = true;
		}
		else 
		{ 
			bShouldRotateHead = false;
		}

		// Get the Character's Camera bcz we need a forward vector of where we are looking at in the viewport
		APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		if (Main && bShouldRotateHead && PlayerCameraManager && Main->EquippedWeapon==nullptr)
		{
			// Get the Character's forward and right vector and Camera's forward vector
			float PlayerFwdVectorYawRotation = Main->GetActorRotation().Yaw;
			float CamManagerYawRotation = PlayerCameraManager->GetCameraRotation().Yaw;
			float PlayerRightVectorYawRotation = Pawn->GetActorRightVector().Rotation().Yaw;
			
			/// For Sideways Rotation ///
			float anglebwFV_PC = FMath::FindDeltaAngleDegrees(PlayerFwdVectorYawRotation, CamManagerYawRotation);

			// bool which is true if our viewport's forward vector is facing away from the player's forward vector
			bool bOutofBounds = false;
			// we need to calculate it here, to see if Character should rotate its head back to start position
			if (FMath::Abs(anglebwFV_PC) >= 135.f)
			{
				bOutofBounds = true;
			}

			anglebwFV_PC *= 0.5f;
			anglebwFV_PC = FMath::ClampAngle(anglebwFV_PC, -30.f, 30.f); // limit player's head rotation b/w 0 to 32 degrees
			
			// Head's Pitch Rotation
			float CameraPitch = PlayerCameraManager->GetCameraRotation().Pitch; //This is used to limit head's pitch rotation.s 
			CameraPitch *= -0.5f;
			if (CameraPitch < 11.f && CameraPitch > 3.f)// in these conditions we don't need to rotate head's pitch
			{
				CameraPitch=0.f;
			}
			else
			{
				CameraPitch = FMath::ClampAngle(CameraPitch, -10.f, 15.f);
			}

			// We are doing this because of orientation of neck bone in local space
			FRotator TargetRotation = FRotator(0.f, anglebwFV_PC, CameraPitch);

			// here we receive the final value of rotation be be used by the bone rotation nodes in anim bp
			NeckRotation = FMath::RInterpTo(NeckRotation, bOutofBounds ? FRotator(0.f) : TargetRotation, GetWorld()->GetDeltaSeconds(), 2.f);
		}
		else
		{
			NeckRotation = FMath::RInterpTo(NeckRotation, FRotator(0.f), GetWorld()->GetDeltaSeconds(), 2.f);
		}


		if (Main->HitImpactPhysics.IsActive())
		{
			Main->HitImpactPhysics.Update(Mesh, GetWorld()->GetDeltaSeconds());
		}
	}
		
}

