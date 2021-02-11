// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "MPhysicalMaterial.generated.h"

/**
 * 
 */

UCLASS()
class BASICS_API UMPhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()
public:
	/** Sound played when the character runs into this surface at speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	USoundBase* HitSound;

	/** Sound played when the character walks beside/into this surface (scuffling sound) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	USoundBase* ScuffSound;

	/** Particle spawned when the character runs into this surface at speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	UParticleSystem* HitParticle;

	/** Particle spawned when the character walks beside/into this surface (scuffling sound) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	UParticleSystem* ScuffParticle;

	/** Maps Velocity [Time] to Volume [Value] when the character runs into this surface at speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects, meta = (DisplayName = "Hit [Velocity -> Volume]"))
	FRuntimeFloatCurve HitVelocityToVolume;

	/** Maps Velocity [Time] to Pitch [Value] when the character runs into this surface at speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects, meta = (DisplayName = "Hit [Velocity -> Pitch]"))
	FRuntimeFloatCurve HitVelocityToPitch;
	
	/** Maps Velocity [Time] to Volume [Value] when the character walks beside/into this surface (scuffling sound) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects, meta = (DisplayName = "Scuff [Velocity -> Volume]"))
	FRuntimeFloatCurve ScuffVelocityToVolume;

	/** Maps Velocity [Time] to Pitch [Value] when the character walks beside/into this surface (scuffling sound) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects, meta = (DisplayName = "Scuff [Velocity -> Pitch]"))
	FRuntimeFloatCurve ScuffVelocityToPitch;

	UMPhysicalMaterial()
		: HitSound(nullptr)
		, ScuffSound(nullptr)
		, HitParticle(nullptr)
		, ScuffParticle(nullptr)
	{
		HitVelocityToVolume.GetRichCurve()->AddKey(300.f, 1.0f);
		HitVelocityToVolume.GetRichCurve()->AddKey(700.f, 1.5f);
		HitVelocityToPitch.GetRichCurve()->AddKey(0.f, 1.0f);

		ScuffVelocityToVolume.GetRichCurve()->AddKey(0.f, 1.0f);
		ScuffVelocityToPitch.GetRichCurve()->AddKey(0.f, 1.0f);
	}
};
