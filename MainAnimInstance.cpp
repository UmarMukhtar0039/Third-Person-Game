// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "Main.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MPhysicalMaterial.h"


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

			if (Mesh)
			{
				Mesh->OnBoneTransformsFinalized.RemoveDynamic(this, &UMainAnimInstance::ComputeSurfaceImpacts);
				Mesh->OnBoneTransformsFinalized.AddDynamic(this, &UMainAnimInstance::ComputeSurfaceImpacts);
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


void UMainAnimInstance::ComputeSurfaceImpacts()
{
	if (!IsValid(Main))
	{
		return;
	}

	const float DeltaTime = GetWorld()->GetDeltaSeconds();

	// ----------------------------------------------------------------------
	// Compute footsteps
	// ----------------------------------------------------------------------
	for (FMISurfaceImpact& Impact : SurfaceImpacts)
	{
		const bool bValidLOD = Impact.LODThreshold == INDEX_NONE || GetLODLevel() <= Impact.LODThreshold;

		if (!Mesh->DoesSocketExist(Impact.Socket))
		{
			UE_LOG(LogTemp, Warning, TEXT("Socket { %s } does not exist on { %s }, can not play impact/footstep"), *Impact.Socket.ToString(), *GetNameSafe(Mesh->SkeletalMesh));
			continue;
		}

		if (!Mesh->DoesSocketExist(Impact.Bone.BoneName))
		{
			UE_LOG(LogTemp, Warning, TEXT("Bone { %s } does not exist on { %s }, can not play impact/footstep"), *Impact.Socket.ToString(), *GetNameSafe(Mesh->SkeletalMesh));
			continue;
		}

		if (bValidLOD)
		{
			const FVector& NewVelocity = Mesh->GetPhysicsLinearVelocity(Impact.Bone.BoneName);

			const FVector& ImpactVelocity = Impact.PreviousVelocity;
			Impact.PreviousVelocity = NewVelocity;

			// On cooldown, no need to process further
			if (GetWorld()->GetTimerManager().IsTimerActive(Impact.CooldownTimerHandle))
			{
				continue;
			}

			const float FootSpeed = ImpactVelocity.Size();
			UE_LOG(LogTemp, Warning, TEXT("Velocity %f"), ImpactVelocity.Size());

			// Insignificant movement
			if (FootSpeed < Impact.MinVelocity)
			{
				return;
			}

			// Trace to the ground
			FHitResult Hit(ForceInit);
			const FVector TraceStart = Mesh->GetSocketLocation(Impact.Socket) + FVector::UpVector * Impact.TraceStartHeight;

			// Create an array of object types to trace
			TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjects;
			for (TEnumAsByte<ECollisionChannel> ObjChannel : Impact.ObjectsToTrace)
			{
				TraceObjects.Add(UEngineTypes::ConvertToObjectType(ObjChannel));
			}

			// Add 2.4f because UMainMovementComponent::MAX_FLOOR_DIST = 2.4f which
			// elevates mesh this far off ground
			const FVector TraceEnd = TraceStart - FVector(0.f, 0.f, Impact.TraceDistFromGround+25.f);

			if (
				UKismetSystemLibrary::LineTraceSingleForObjects(
					GetWorld(),
					TraceStart,
					TraceEnd,
					TraceObjects,
					true,
					TArray<AActor*>() = { Main },
#if WITH_EDITORONLY_DATA
					(Impact.bDebugDrawTrace ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None),
#else
					EDrawDebugTrace::None,
#endif
					Hit,
					false
				))
			{

				// Must have left the ground before it can re-trigger
				if (!Impact.bWasHit)
				{
					// Triggered, start cooldown timer
					GetWorld()->GetTimerManager().SetTimer(Impact.CooldownTimerHandle, Impact.MinTriggerInterval, false);

					UMPhysicalMaterial* PhysMat = Hit.PhysMaterial.IsValid() ? Cast<UMPhysicalMaterial>(Hit.PhysMaterial.Get()) : nullptr;
					if (!PhysMat) { PhysMat = Main->DefaultPhysicalMaterial; }
					if (PhysMat)
					{
						// Play sound effect
						USoundBase* const SoundToPlay = PhysMat->BoneImpactSound;
						if (SoundToPlay)
						{
							const FRuntimeFloatCurve& VolumeCurve = PhysMat->BoneImpactVelocityToVolume;
							const FRuntimeFloatCurve& PitchCurve = PhysMat->BoneImpactVelocityToPitch;
							const float Volume = VolumeCurve.GetRichCurveConst()->Eval(FootSpeed);
							const float Pitch = PitchCurve.GetRichCurveConst()->Eval(FootSpeed);

							if (Volume > 0.f && Pitch > 0.f)
							{
								UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, Hit.ImpactPoint, Volume, Pitch);
							}
						}

						// Play particle effect
						UParticleSystem* const ParticleToPlay = PhysMat != nullptr ? PhysMat->BoneImpactParticle : nullptr;
						if (ParticleToPlay)
						{
							const FTransform ParticleTransform((-ImpactVelocity.GetSafeNormal()).Rotation(), Hit.ImpactPoint, FVector::OneVector);
							UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleToPlay, ParticleTransform, true, EPSCPoolMethod::AutoRelease);
						}
					}
				}
			}

			Impact.bWasHit = Hit.bBlockingHit;
		}
	}
}
