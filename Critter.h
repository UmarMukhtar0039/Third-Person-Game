// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Critter.generated.h"

UCLASS()
class BASICS_API ACritter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACritter();
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USpringArmComponent* springarm;

	UPROPERTY(EditInstanceOnly, Category = "Movement")
	FVector CurrentVelocity;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Movement")
	float speed;
private:
	void Forward(float input);
	void Right(float input);



protected:
	// Called when the game starts or when spawned
	 void BeginPlay() override;

public:	
	// Called every frame
	void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



};
