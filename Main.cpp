// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "FirstSaveGame.h"
#include "ItemStorage.h"
//#include "Components/PoseableMeshComponent.h"

// Sets default values
AMain::AMain()
{
	
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; // Camera 600 units far
	CameraBoom->bUsePawnControlRotation = true; //Rotate Arm Based on controller
	//Hard Coding Capsule collision component	
	GetCapsuleComponent()->SetCapsuleSize(48.f,105.f);
	//Making Player Turn and Move in direction where camera is pointing
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.f); //... at this rotation rate i.e. 540.f
	GetCharacterMovement()->JumpZVelocity = 650.f; // Giving velocity of jump
	GetCharacterMovement()->AirControl = 0.2f; // Giving control in air
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//Attach the camera to the end of the boom and let the boom adjust to match the controller orientation.
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//Set our turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	//Don't rotate when the controller rotate 
	//Let it only affect camera.
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	//setting up stats
	MaxHealth = 100.f;
	Health = 100.f;
	Stamina = 150.f;
	MaxStamina = 150.f;
	Coins = 0;
	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;
	StaminaDrainRate = 25.f;
	MinSprintStamina = 50.f;
	bShiftKeyDown = false;
	bESCDown=false;

	//init ENUMs
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	bisMovingForward = false;
	bisMovingRight = false;
	EquippedWeapon = nullptr;
	ActiveOverlappingItem = nullptr;
	bLMBDown = false;
	bAttacking = false;
	InterpSpeed = 15.f;
	bInterpToEnemy = false;
	bHasCombatTarget=false;
}

// Called when the game starts or when spawned
void AMain::BeginPlay() 
{
	Super::BeginPlay();

	MainPlayerController = Cast<AMainPlayerController>(GetController());

	FString Map = GetWorld()->GetMapName();
	Map.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if (Map != "SunTemple")
	{
		LoadGameNoSwitch();
		if (MainPlayerController)
		{
			MainPlayerController->GameModeOnly();
		}
	}

}


// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(MovementStatus == EMovementStatus::EMS_Dead) return;

	float DeltaStamina = StaminaDrainRate * DeltaTime;
	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal :
		if (bShiftKeyDown) //if the shift key is pressed and character is not moving then it shouldn't drain stamina
		{
			if (bisMovingForward || bisMovingRight) // if character is moving
			{
				Stamina -= DeltaStamina;
				if (Stamina <= MinSprintStamina)
				{
					SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				}
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			else // if character is not moving
			{
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
		}
		else // shift key up
		{
			if (Stamina + DeltaStamina < MaxStamina) // if stamina is less than max increment with DS
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	
	case EStaminaStatus::ESS_BelowMinimum :
		if (bShiftKeyDown)
		{
			Stamina -= DeltaStamina;
			if (Stamina <= 0.f)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else if (bisMovingForward || bisMovingRight)
			{
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			else
			{
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
		}
		else //shift key up
		{
			Stamina += DeltaStamina;
			if (Stamina >= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;

	case EStaminaStatus::ESS_Exhausted:
		if (bShiftKeyDown)
		{
			Stamina = 0.f;
		}
		else 
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	case EStaminaStatus::ESS_ExhaustedRecovering:
		Stamina += DeltaStamina;
		if (Stamina >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	default:
		break;
	}

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation =  FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);
		
		SetActorRotation(InterpRotation);
	}

	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}

}


// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//Check if player input component is valid if not it will stop here unless it is valid
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMain::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &AMain::MoveRight);
	
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMain::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AMain::Turn);

	PlayerInputComponent->BindAxis(TEXT("TurnRate"), this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AMain::LookUpAtRate);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);	
	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMain::ESCDown);
}

bool AMain::CanMove(float value)
{
	if (MainPlayerController)
	{
		return ((value != 0.f) && !bAttacking 
		&& MovementStatus != EMovementStatus::EMS_Dead 
		&& !MainPlayerController->bPauseMenuVisible);
	}

	return false;
}

void AMain::MoveForward(float value)
{
	bisMovingForward = false;
	if(CanMove(value))
	{ // Find the Direction to move when we rotate camera
		const FRotator Rotation = Controller->GetControlRotation();
	//Here Yaw of YawRotation is given by Controller's Yaw.
		const FRotator YawRotation(0.f,Rotation.Yaw,0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction,value);

		bisMovingForward = true;
	}
}

void AMain::MoveRight(float value)
{
	bisMovingRight = false;
		if(CanMove(value))
	{ // Find the Direction to move when we rotate camera
		const FRotator Rotation = Controller->GetControlRotation();
	//Here Yaw of YawRotation is given by Controller's Yaw.
		const FRotator YawRotation(0.f,Rotation.Yaw,0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction,value);
		
		bisMovingRight = true;
	}
}


void AMain::Turn(float value)
{
	if (CanMove(value))
	{
		AddControllerYawInput(value);
	}
}

void AMain::LookUp(float value)
{
	if (CanMove(value))
	{
		AddControllerPitchInput(value);
	}
}

void AMain::TurnAtRate(float rate)
{
	//UE_LOG(LogTemp, Warning, TEXT("TurnRate %f"), rate* BaseTurnRate* GetWorld()->GetDeltaSeconds());
	AddControllerYawInput(rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpAtRate(float rate)
{
	//UE_LOG(LogTemp, Warning, TEXT("LookupRate %f"), FMath::Clamp(rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds(),-.5f,.5f));
	AddControllerPitchInput(FMath::Clamp(rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds(),-.5f,.5f));

	// Just Trying out Head Rotation
	//const FRotator Rotation = Controller->GetControlRotation();
	////Here Yaw of YawRotation is given by Controller's Yaw.
	//const FRotator PitchRotation(Rotation.Yaw,0.f, 0.f);
	//const FVector Direction = FRotationMatrix(PitchRotation).GetUnitAxis(EAxis::X);
	//GetMesh()->GetSocketTransform(TEXT("HeadSocket")).TransformPosition(Direction);

	//auto s = GetMesh()->GetAnimInstance();
	//const FRotator Fr = Controller->GetControlRotation();

}


float AMain::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Health -= DamageAmount;
	if (Health <= 0.f)
	{
		Die();
		if (DamageCauser)
		{	
			AEnemy* Enemy= Cast<AEnemy>(DamageCauser);
			if (Enemy)
			{
				Enemy->bHasValidTarget=false;
			}
		}
	}
	
	return DamageAmount;
}

void AMain::Die()
{
	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	UAnimInstance* AnimInstace = GetMesh()->GetAnimInstance();
	if (AnimInstace && CombatMontage)
	{
		AnimInstace->Montage_Play(CombatMontage, 1.f); 
		AnimInstace->Montage_JumpToSection(FName("Death"));
	}
	SetMovementStatus(EMovementStatus::EMS_Dead);
}

void AMain::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	//GetMesh()->GetAnimInstance()->Montage_Pause(CombatMontage);
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMain::Jump()
{
	if (MainPlayerController) if (MainPlayerController->bPauseMenuVisible) return;
	
	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		Super::Jump();
	}
}

void AMain::IncrementCoin(int32 Amount)
{
	Coins += Amount;
}


void AMain::IncrementHealth(float Amount)
{
	Health +=Amount;
	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}
}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);

	return LookAtRotationYaw;
}

void AMain::SetMovementStatus(EMovementStatus status)
{
	MovementStatus = status;

	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else // if state is running
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}	
}

void AMain::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void AMain::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

// For Debugging
void AMain::showPickupLocations()
{
	for (const FVector& Locations : PickupLocations)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, Locations, 25.f, 12, FLinearColor::Green, 5.f, 2.f);
	}
}

void AMain::SetEquippedWeapon(AWeapon * WeaponToSet)
{
	if (EquippedWeapon) //if there is an already equipped weapon then destroy the old one to equip new
	{
		EquippedWeapon->Destroy();
	}

	EquippedWeapon = WeaponToSet;
}
[[]]
void AMain::LMBDown()
{
	bLMBDown = true;
	
	if (MovementStatus == EMovementStatus::EMS_Dead) return;
	
	if (MainPlayerController) if(MainPlayerController->bPauseMenuVisible) return;

	if (ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
		}
	}

	else if (EquippedWeapon)
	{
		Attack();
	}

}

void AMain::LMBUp()
{
	bLMBDown = false;
}

void AMain::Attack()
{
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead)
	{
		bAttacking = true;
		SetInterpToEnemy(true);

		UAnimInstance* AnimInstace = GetMesh()->GetAnimInstance();
		if (AnimInstace && CombatMontage)
		{
			uint32 Section = FMath::RandRange(0, 1);

			switch (Section)
			{
			case 0:
				AnimInstace->Montage_Play(CombatMontage, 2.f);
				//UE_LOG(LogTemp, Warning, TEXT("playing length %f"),len);
				AnimInstace->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				break;
			case 1:
				AnimInstace->Montage_Play(CombatMontage, 2.f);
				//UE_LOG(LogTemp, Warning, TEXT("playing length %f"),len);
				AnimInstace->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				break;
			default:
				break;
			}

		}

	}
}

void AMain::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false);
	if (bLMBDown)
	{
		Attack();
	}
}



void AMain::PlaySwordSwingSound()
{
	if (EquippedWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}


void AMain::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

void AMain::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;
	
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num()==0) 
	{
		if	(MainPlayerController) 		MainPlayerController->RemoveEnemyHealthBar(); 
				return;
	}

	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);
	if (ClosestEnemy)
	{
		FVector Location = GetActorLocation();
		float MinDis = (ClosestEnemy->GetActorLocation() - Location).Size();
		
		for (AActor* Actor : OverlappingActors)
		{	
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			float DisToActor = (Enemy->GetActorLocation() - Location).Size();
			if (DisToActor < MinDis)
			{
				MinDis = DisToActor;
				ClosestEnemy = Enemy;
			}
		}

		if (MainPlayerController)
		{
			MainPlayerController->DisplayEnemyHealthBar();
		}

		SetCombatTarget(ClosestEnemy);
		bHasCombatTarget=true;
	}
}

void AMain::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevel = World->GetMapName();
		FName CurrentLevelName(*CurrentLevel);
		if (CurrentLevelName != LevelName)
		{
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}

void AMain::SaveGame()
{
	UFirstSaveGame* SaveGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	SaveGameInstance->CharacterStats.Coins=Coins;
	SaveGameInstance->CharacterStats.Health=Health;
	SaveGameInstance->CharacterStats.MaxHealth=MaxHealth;
	SaveGameInstance->CharacterStats.Stamina=Stamina;
	SaveGameInstance->CharacterStats.MaxStamina=MaxStamina;
	SaveGameInstance->CharacterStats.Location=GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation=GetActorRotation();

	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	SaveGameInstance->CharacterStats.LevelName=MapName;

	if (EquippedWeapon)
	{
		SaveGameInstance->CharacterStats.WeaponName = EquippedWeapon->Name;
	}

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);
}

void AMain::LoadGame(bool SetPosition)
{
	UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	
	LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));
	
	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	Coins = LoadGameInstance->CharacterStats.Coins;
	Health = LoadGameInstance->CharacterStats.Health;
	
	if (WeaponStorage)
	{
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons)
		{
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;
			if (WeaponName != TEXT(""))
			{
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
				WeaponToEquip->Equip(this);
			}
		}
	}
	
	if (SetPosition)
	{
		SetActorLocation(LoadGameInstance->CharacterStats.Location);
		SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
	}
	
	SetMovementStatus(EMovementStatus::EMS_Normal);
	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;
	
	if (LoadGameInstance->CharacterStats.LevelName != TEXT(""))
	{
		FName LevelName(*LoadGameInstance->CharacterStats.LevelName);
		SwitchLevel(LevelName);
	}
}

void AMain::LoadGameNoSwitch()
{
	UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	Coins = LoadGameInstance->CharacterStats.Coins;
	Health = LoadGameInstance->CharacterStats.Health;

	if (WeaponStorage)
	{
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons)
		{
			AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[LoadGameInstance->CharacterStats.WeaponName]);
			WeaponToEquip->Equip(this);
		}
	}

	SetMovementStatus(EMovementStatus::EMS_Normal);
	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;
}

void AMain::ESCDown()
{
	bESCDown=true;
	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu();
	}
}

void AMain::ESCUp()
{
	bESCDown=false;
}
