// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UCLASS()
class BASICS_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//AllowPrivateAccesss means this Variable can be accessed by this blueprint but not any other
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	/** Camera Boom positioning the camera behind the player*/
	class USpringArmComponent* CameraBoom;
	
	/** Follow Camera*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Base turn rates to scale turning functions for the camera*/ 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate;	

	FORCEINLINE class USpringArmComponent* GetCameraBoom()const {return CameraBoom;}
	FORCEINLINE class UCameraComponent* GetFollowCamera()const { return FollowCamera; }
	
	// called for forwards and backwards input
	void MoveForward(float value);
	//called for right and left inputs
	void MoveRight(float value);
	
	/**called via input to turn at a given rate
	 * @param Rate this is a normalized rate i.e. 1.0 means 100% of desired turn rate 
	 */
	void TurnAtRate(float rate);
	/**called via input to turn at a given rate
	 * @param Rate this is a normalized rate i.e. 1.0 means 100% of desired lookup/down rate 
	 */
	void LookUpAtRate(float rate);

};
