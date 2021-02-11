#pragma once

#include "AlphaBlend.h"
#include "MyTypes.generated.h"

class UBlendSpaceBase;

UENUM(BlueprintType)
enum class EPhysicsBlendState : uint8
{
	PBS_Inactive					UMETA(DisplayName = "Inactive"),
	PBS_In							UMETA(DisplayName = "In"),
	PBS_Out							UMETA(DisplayName = "Out"),
	PBS_MAX = 255						UMETA(Hidden),
};

USTRUCT(BlueprintType)
struct FPhysicsBlend
{
	GENERATED_BODY()

	FPhysicsBlend()
		: BoneName(NAME_None)
		, ImpulseMultiplier(1.f)
		, BlendIn(FAlphaBlend())
		, BlendOut(FAlphaBlend())
		, MinBlendWeight(0.f)
		, MaxBlendWeight(1.f)
		, MaxImpulseTaken(1000.f)
		, ImpulseInterpOutRate(10.f)
		, PhysicsBlendState(EPhysicsBlendState::PBS_Inactive)
		, CurrentImpulse(FVector::ZeroVector)
	{
		Init();
	}

	FPhysicsBlend(const FName& InBoneName, float InImpulseMultiplier = 1.f, float InMinBlendWeight = 0.f, float InMaxBlendWeight = 1.f)
		: BoneName(InBoneName)
		, ImpulseMultiplier(InImpulseMultiplier)
		, BlendIn(FAlphaBlend())
		, BlendOut(FAlphaBlend())
		, MinBlendWeight(InMinBlendWeight)
		, MaxBlendWeight(InMaxBlendWeight)
		, MaxImpulseTaken(1000.f)
		, ImpulseInterpOutRate(10.f)
		, PhysicsBlendState(EPhysicsBlendState::PBS_Inactive)
		, CurrentImpulse(FVector::ZeroVector)
	{
		Init();
	}

	/** 
	 * Bone to simulate physics on 
	 * Impulse application will fail if physics asset does not contain a body (capsule, box, sphere, etc)
	 * @note : Set to None to disable
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Physics)
	FName BoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Physics)
	float ImpulseMultiplier;

	/** Parameters for blending in */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Physics)
	FAlphaBlend BlendIn;

	/** Parameters for blending out */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Physics)
	FAlphaBlend BlendOut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Physics, meta = (UIMin = "0", ClampMin="0", UIMax="1", ClampMax="1"))
	float MinBlendWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Physics, meta = (UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1"))
	float MaxBlendWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Physics, meta = (UIMin = "0", ClampMin = "0"))
	float MaxImpulseTaken;

	/**
	 * How fast the applied impulse interpolates out 
	 * Workaround for not knowing the impulse on each bone but needing to limit total application
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Physics, meta = (UIMin = "0", ClampMin = "0"))
	float ImpulseInterpOutRate;

	UPROPERTY(BlueprintReadOnly, Category = Physics)
	EPhysicsBlendState PhysicsBlendState;

protected:
	FVector CurrentImpulse;

public:
	void Impact(USkeletalMeshComponent* const Mesh, const FVector& ImpactNormal, const float ImpactMagnitude);

	/** @return Has completed */
	bool Update(USkeletalMeshComponent* const Mesh, float DeltaTime);

	FORCEINLINE bool IsActive() const { return PhysicsBlendState > EPhysicsBlendState::PBS_Inactive && PhysicsBlendState < EPhysicsBlendState::PBS_MAX; }

	bool CanSimulate(USkeletalMeshComponent* const Mesh) const;

private:
	FORCEINLINE void Init()
	{
		BlendIn.SetAlpha(1.f);
		BlendOut.SetAlpha(1.f);
		BlendIn.SetValueRange(0.f, 1.f);
		BlendOut.SetValueRange(1.f, 0.f);
	}
};

USTRUCT(BlueprintType)
struct FMShotImpact
{
	GENERATED_BODY()

		FMShotImpact()
	{}

	TMap<FName, FPhysicsBlend> BoneMap;
};
