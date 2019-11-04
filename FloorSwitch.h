// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

UCLASS()
class BASICS_API AFloorSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloorSwitch();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:
	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
	FVector InitialDoorLocation;
	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
	FVector InitialSwitchLocation;

	FTimerHandle SwitchHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	float SwitchTime;

	bool bCharacterOnSwitch;
public: 
	/**Overlap volume for the floor functionality to be triggered*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	class UBoxComponent* TriggerBox;

	//This is mesh for floor switch on which player will step on
	UPROPERTY(VisibleAnywhere, Category = "FloorSwitch")
	class UStaticMeshComponent* FloorSwitch;
	
	//Door mesh
	UPROPERTY(VisibleAnywhere, Category = "FloorSwitch")
	class UStaticMeshComponent* Door;
	
	UFUNCTION(BlueprintImplementableEvent,Category = "FloorSwitch")
	void RaiseDoor();
	
	UFUNCTION(BlueprintImplementableEvent,Category = "FloorSwitch")
	void LowerDoor();	
	
	UFUNCTION(BlueprintImplementableEvent,Category = "FloorSwitch")
	void RaiseFloorSwitch();
	
	UFUNCTION(BlueprintImplementableEvent,Category = "FloorSwitch")
	void LowerFloorSwitch();

	UFUNCTION(BlueprintCallable, Category = "FloorSwitch")
	void UpdateDoorLocation(float Z);

	UFUNCTION(BlueprintCallable, Category = "FloorSwitch")
	void UpdateFloorSwitchLocation(float Z);
	
	void CloseDoor();
};
