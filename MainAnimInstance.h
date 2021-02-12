// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

USTRUCT(BlueprintType)
struct FMISurfaceImpact
{
	GENERATED_BODY()

		FMISurfaceImpact()
		: Bone(FBoneReference())
		, Socket(NAME_None)
		, MinVelocity(30.f)
		, TraceStartHeight(5.f)
		, TraceDistFromGround(5.f)
		, MinTriggerInterval(0.2f)
		, bRagdollOnly(false)
		, LODThreshold(2)
		, bWasHit(false)
		, PreviousVelocity(0.f)
	{
		ObjectsToTrace.Add(ECollisionChannel::ECC_WorldStatic);
		ObjectsToTrace.Add(ECollisionChannel::ECC_WorldDynamic);
	}

	FMISurfaceImpact(const FBoneReference& InBone, const FName& InSocket, const float InMinVelocity = 30.f, const bool bInRagdollOnly = false, const float InTraceDistFromGround = 5.f, const float InTraceStartHeight = 5.f)
		: Bone(InBone)
		, Socket(InSocket)
		, MinVelocity(InMinVelocity)
		, TraceStartHeight(InTraceStartHeight)
		, TraceDistFromGround(InTraceDistFromGround)
		, MinTriggerInterval(0.2f)
		, bRagdollOnly(bInRagdollOnly)
		, LODThreshold(2)
		, bWasHit(false)
		, PreviousVelocity(0.f)
	{
		ObjectsToTrace.Add(ECollisionChannel::ECC_WorldStatic);
		ObjectsToTrace.Add(ECollisionChannel::ECC_WorldDynamic);
	}

	/** The bone that is used to compute velocity */
	UPROPERTY(EditAnywhere, Category = Impact)
		FBoneReference Bone;

	/** The socket that is used to trace to the ground and to spawn effects from */
	UPROPERTY(EditAnywhere, Category = Impact)
		FName Socket;

	/** How fast the bone must be moving to trigger a Impact */
	UPROPERTY(EditAnywhere, Category = Impact)
		float MinVelocity;

	/** How far above the socket to start the trace from */
	UPROPERTY(EditAnywhere, Category = Impact)
		float TraceStartHeight;

	/** How far above the ground the bone can be while still triggering an impact */
	UPROPERTY(EditAnywhere, Category = Impact)
		float TraceDistFromGround;

	/** How often an impact can be triggered by this bone */
	UPROPERTY(EditAnywhere, Category = Impact)
		float MinTriggerInterval;

	/** If true, will only trigger when ragdoll is active */
	UPROPERTY(EditAnywhere, Category = Impact)
		bool bRagdollOnly;

	/*
	* Max LOD that this impact is allowed to run
	* For example if you have LODThreadhold to be 2, it will run until LOD 2 (based on 0 index)
	* when the component LOD becomes 3, it will stop
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Impact, meta = (DisplayName = "LOD Threshold"))
		int32 LODThreshold;

	UPROPERTY(EditAnywhere, Category = Impact)
		TArray<TEnumAsByte<ECollisionChannel>> ObjectsToTrace;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = Impact)
		bool bDebugDrawTrace = false;
#endif

	bool bWasHit;

	FTimerHandle CooldownTimerHandle;

	FVector PreviousVelocity;
};

/**
 * 
 */
UCLASS()
class BASICS_API UMainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	//we override this virtual function
	void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable,Category = "Animation Properties")
	void UpdateAnimationProperties();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsInAir;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	class APawn* Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MainCharacter)
	class AMain* Main;

	UPROPERTY(BlueprintReadWrite, Category = References)
	USkeletalMeshComponent* Mesh;

	/** A physically accurate footstep system that can handle any and all other bones hitting the ground */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	TArray<FMISurfaceImpact> SurfaceImpacts;

public:
	UMainAnimInstance()
	{
		const FMISurfaceImpact LeftFootImpact = FMISurfaceImpact(FBoneReference("LeftFoot"), TEXT("footstep_l"));
		const FMISurfaceImpact RightFootImpact = FMISurfaceImpact(FBoneReference("RightFoot"), TEXT("footstep_r"));
		/*const FMISurfaceImpact LeftHandImpact = FMISurfaceImpact(FBoneReference("hand_l"), TEXT("hand_l"), 5.f, true, 15.f);
		const FMISurfaceImpact RightHandImpact = FMISurfaceImpact(FBoneReference("hand_r"), TEXT("hand_r"), 5.f, true, 15.f);
		const FMISurfaceImpact HeadImpact = FMISurfaceImpact(FBoneReference("head"), TEXT("head"), 5.f, true, 25.f);*/
		//SurfaceImpacts = { LeftFootImpact, RightFootImpact, LeftHandImpact, RightHandImpact, HeadImpact };
		SurfaceImpacts = { LeftFootImpact, RightFootImpact };

	}

	/** A glorified physically accurate footstep system that can handle any and all other bones hitting the ground */
	UFUNCTION()
	void ComputeSurfaceImpacts();

public:
	//Head Rotation
	bool bShouldRotateHead;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Neck Movement")
	FRotator NeckRotation;
};
