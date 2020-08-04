// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),
	
	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),
	
	ESS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class BASICS_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	AMain();
	/** For Debugging the pick locations*/
	TArray<FVector> PickupLocations;
	void showPickupLocations();
	
	// Sets default values for this character's properties

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Enums")
	EMovementStatus MovementStatus;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	FORCEINLINE void SetStaminaStatus(EStaminaStatus status){ StaminaStatus = status; }
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	/**********************************/
	/** for Interpolating the Character to the target while attacking*/
	float InterpSpeed;
	bool bInterpToEnemy;
	void SetInterpToEnemy(bool Interp);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Target){ CombatTarget = Target;}

	FRotator GetLookAtRotationYaw(FVector Target);

	/**********************************/


	/** Sets movement status and running speed */
	void SetMovementStatus(EMovementStatus status);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//class UPoseableMeshComponent* p;
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
	
	bool CanMove(float value);
	// needed to know when to exhaust stamina 
	bool bisMovingForward;
	bool bisMovingRight;

	// called for Yaw Rotation
	void Turn(float value);
	// called for Pitch Rotation
	void LookUp(float value);
	/**called via input to turn at a given rate
	 * @param rate: this is a normalized rate i.e. 1.0 means 100% of desired turn rate 
	 */
	void TurnAtRate(float rate);
	
	/**called via input to turn at a given rate
	 * @param rate: this is a normalized rate i.e. 1.0 means 100% of desired lookup/down rate 
	 */
	void LookUpAtRate(float rate);

	// Running and Sprinting
	bool bShiftKeyDown;

	/** pressed down to enable sprinting*/
	void ShiftKeyDown();

	/** Released to stop sprinting*/
	void ShiftKeyUp();
	/**
	 * 
	 * Player Stats
	 *
	*/
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStats")
	float Stamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStats")
	int32 Coins;

	void DecrementHealth(float Amount);
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void Die();
	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	void Jump() override;

	UFUNCTION(BlueprintCallable)
	void IncrementCoin(int32 Amount);
	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;

	//Weapons:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Items")
	class AWeapon* EquippedWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	class AItem* ActiveOverlappingItem;

	void SetEquippedWeapon(AWeapon* WeaponToSet);
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }
	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }
	
	bool bLMBDown;
	void LMBDown();
	void LMBUp();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Anims")
	bool bAttacking;
	void Attack();
	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Anims")
	class UAnimMontage* CombatMontage;
	
	UFUNCTION(BlueprintCallable)
	void PlaySwordSwingSound();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* HitParticles;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundCue* HitSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat)
	bool bHasCombatTarget;

	FORCEINLINE void SetHasCombatTarget(bool HasTarget){ bHasCombatTarget=HasTarget;}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Controller")
	class AMainPlayerController* MainPlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	FVector CombatTargetLocation;

	void UpdateCombatTarget();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	TSubclassOf<AEnemy> EnemyFilter; 

	void SwitchLevel(FName LevelName);

	UFUNCTION(BlueprintCallable)
	void SaveGame();
	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetPosition);

	UFUNCTION(BlueprintCallable)
	void LoadGameNoSwitch();

	UPROPERTY(EditDefaultsOnly, Category = SavedData)
	TSubclassOf<class AItemStorage> WeaponStorage;

	bool bESCDown;
	void ESCDown();
	void ESCUp();
};
