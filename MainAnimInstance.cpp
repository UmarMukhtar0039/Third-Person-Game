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
			FVector PlayerFwdVector = Main->GetActorForwardVector();
			FVector CamManagerFwdVector = PlayerCameraManager->GetActorForwardVector();
			FVector PlayerRightVector = Pawn->GetActorRightVector();
			
			/// For Sideways Rotation ///
			//Convert the player's forward vector into 2D because for sideways rotation, we just need x,y axis.
			FVector2D PlayerFwdVector2D(PlayerFwdVector.X, PlayerFwdVector.Y);
			float sizeofPlayerFwdVector = PlayerFwdVector2D.Size();

			FVector2D CamManagerFwdVector2D(CamManagerFwdVector.X, CamManagerFwdVector.Y);
			float sizeofCameraFwdVector = CamManagerFwdVector2D.Size();
			float DotProduct = FVector2D::DotProduct(PlayerFwdVector2D, CamManagerFwdVector2D);
			float TotalSize = sizeofCameraFwdVector * sizeofPlayerFwdVector; // camera Fwd Vector's size might increase or decrease but player's vector's size won't change

			float anglebwFV_PC = FMath::Acos(DotProduct / TotalSize);
			anglebwFV_PC = FMath::RadiansToDegrees(anglebwFV_PC);
			
			// bool which is true if our viewport's forward vector is facing away from the player's forward vector
			bool bOutofBounds = false;
			// we need to calculate it here, to see if Character should rotate its head back to start position
			if (anglebwFV_PC >= 155.f)
				bOutofBounds = true;

			anglebwFV_PC *= 0.5;
			anglebwFV_PC = FMath::ClampAngle(anglebwFV_PC, 0.f, 32.f); // limit player's head rotation b/w 0 to 32 degrees
			
			FVector2D PlayerRightVector2D(PlayerRightVector.X, PlayerRightVector.Y);
			float sizeofPlayerRightVector = PlayerRightVector2D.Size();
			DotProduct = FVector2D::DotProduct(CamManagerFwdVector2D, PlayerRightVector2D);
			TotalSize = sizeofPlayerRightVector * sizeofCameraFwdVector;// sizeofPlayerRightVector will always be unity but sizeofCameraFwdVector might change therefore we leave it as it is.

			float anglebwFVCM_RVP = FMath::Acos(DotProduct / TotalSize);
			anglebwFVCM_RVP = FMath::RadiansToDegrees(anglebwFVCM_RVP);
			
			//When angle is less than 90 degrees it means camera is looking right of player else we negate it to differentiate b/w left and right. 
			if (anglebwFVCM_RVP > 90.f)
			{
				anglebwFV_PC *= -1;
			}

			//Head's Pitch Rotation
			float CameraPitch = PlayerCameraManager->GetCameraRotation().Pitch; //This is used to limit head's pitch rotation.s 
			CameraPitch *= -0.5;
			if (CameraPitch < 11.f && CameraPitch>3.f)// in these conditions we don't need to rotate head's pitch
			{
				CameraPitch=0.f;
			}
			else
			{
				CameraPitch = FMath::ClampAngle(CameraPitch, -10.f, 15.f);
				UE_LOG(LogTemp, Warning, TEXT("Pitch %f"), CameraPitch);
			}

			// We are doing this because of orientation of neck bone in local space
			FRotator TargetRotation = FRotator(0.f, anglebwFV_PC, CameraPitch);

			NeckRotation = FMath::RInterpTo(NeckRotation, bOutofBounds ? FRotator(0.f) : TargetRotation, GetWorld()->GetDeltaSeconds(), 2.f);
		}
		else
		{
			NeckRotation = FMath::RInterpTo(NeckRotation, FRotator(0.f), GetWorld()->GetDeltaSeconds(), 2.f);
		}
	}
		
}

