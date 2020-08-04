// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class BASICS_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	class UBoxComponent* SpawningBox;
	
	UFUNCTION(BlueprintPure, Category = "Spawning")
	FVector GetSpawnPoint();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning | Object")
	TSubclassOf<AActor> Actor1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning | Object")
	TSubclassOf<AActor> Actor2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning | Object")
	TSubclassOf<AActor> Actor3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning | Object")
	TSubclassOf<AActor> Actor4;
	
	TArray< TSubclassOf<AActor>> SpawnArray;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spawning | Object" )
	void SpawnOurActor(UClass* ToSpawn, const FVector& Location);

	UFUNCTION(BlueprintPure, Category = "Spawning")
	TSubclassOf<AActor> GetSpawnActor();
};