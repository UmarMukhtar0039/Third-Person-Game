#include "MyTypes.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"

void FPhysicsBlend::Impact(USkeletalMeshComponent* const Mesh, const FVector& ImpactNormal, const float ImpactMagnitude)
{
	if (CanSimulate(Mesh))
	{
		BlendIn.Reset();
		BlendOut.Reset();

		Mesh->SetAllBodiesBelowSimulatePhysics(BoneName, true);
		Mesh->SetAllBodiesBelowPhysicsBlendWeight(BoneName, MinBlendWeight);

		FVector Impulse = ImpactNormal * ImpactMagnitude * ImpulseMultiplier;
		CurrentImpulse += Impulse;
		CurrentImpulse = CurrentImpulse.GetClampedToMaxSize(MaxImpulseTaken);
		Mesh->AddAngularImpulseInRadians(Impulse, BoneName, false);
		//Mesh->AddImpulse(CurrentImpulse, BoneName, false);

		PhysicsBlendState = EPhysicsBlendState::PBS_In;
	}
}

bool FPhysicsBlend::Update(USkeletalMeshComponent* const Mesh, float DeltaTime)
{
	if (!CurrentImpulse.IsNearlyZero())
	{
		CurrentImpulse = FMath::VInterpConstantTo(CurrentImpulse, FVector::ZeroVector, DeltaTime, ImpulseInterpOutRate);
	}
	else
	{
		CurrentImpulse = FVector::ZeroVector;
	}

	if (IsActive() && Mesh)
	{
		const bool bBlendIn = PhysicsBlendState == EPhysicsBlendState::PBS_In;
		FAlphaBlend& Blend = (bBlendIn) ? BlendIn : BlendOut;
		Blend.Update(DeltaTime);

		if (Blend.IsComplete())
		{
			if (bBlendIn)
			{
				PhysicsBlendState = EPhysicsBlendState::PBS_Out;
			}
			else
			{
				PhysicsBlendState = EPhysicsBlendState::PBS_Inactive;
				Mesh->SetAllBodiesBelowPhysicsBlendWeight(BoneName, 0.f);

				CurrentImpulse = FVector::ZeroVector;

				return true;
			}
		}

		const float Alpha = (bBlendIn) ? Blend.GetAlpha() : 1.f - Blend.GetAlpha();
		const float BlendWeight = UKismetMathLibrary::MapRangeClamped(Alpha, 0.f, 1.f, MinBlendWeight, MaxBlendWeight);

		Mesh->SetAllBodiesBelowPhysicsBlendWeight(BoneName, BlendWeight);
	}

	// Is completed
	return !IsActive() && CurrentImpulse.IsZero();
}

bool FPhysicsBlend::CanSimulate(USkeletalMeshComponent* const Mesh) const
{
	if (!Mesh)
	{
		return false;
	}

	if (!Mesh->DoesSocketExist(BoneName))
	{
		return false;
	}

	if (!ensureMsgf((Mesh->GetCollisionEnabled() == ECollisionEnabled::PhysicsOnly || Mesh->GetCollisionEnabled() == ECollisionEnabled::QueryAndPhysics), TEXT("{ %s } Invalid Collision Enabled - Must simulate physics!"), *Mesh->GetName()))
	{
		return false;
	}

	return true;
}
