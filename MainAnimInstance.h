// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

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


public:
	//Head Rotation
	bool bShouldRotateHead;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation | Neck Movement")
	FRotator NeckRotation;
};
