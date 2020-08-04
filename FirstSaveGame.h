// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FirstSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = SavedGameData)
	float Health;
	
	UPROPERTY(VisibleAnywhere, Category = SavedGameData)
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = SavedGameData)
	float Stamina;

	UPROPERTY(VisibleAnywhere, Category = SavedGameData)
	float MaxStamina;
	
	UPROPERTY(VisibleAnywhere, Category = SavedGameData)
	uint32 Coins;

	UPROPERTY(VisibleAnywhere, Category = SavedGameData)
	FVector Location;

	UPROPERTY(VisibleAnywhere, Category = SavedGameData)
	FRotator Rotation;

	UPROPERTY(VisibleAnywhere, Category = SavedGameData)
	FString WeaponName;
	
	UPROPERTY(VisibleAnywhere, Category = SavedGameData)
	FString LevelName;
};

/**
 * 
 */
UCLASS()
class BASICS_API UFirstSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UFirstSaveGame();

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString PlayerName;
	
	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;
	
	float Health;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FCharacterStats CharacterStats;

};
