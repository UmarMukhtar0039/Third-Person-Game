// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Collider.generated.h"

UCLASS()
class UDEMY_BASICS_API ACollider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACollider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	


	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		class UStaticMeshComponent* meshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		class USphereComponent* sphereComponent;

	FORCEINLINE UStaticMeshComponent* getMeshComponent()const { return meshComponent; }
	FORCEINLINE void SetMeshComponent(UStaticMeshComponent* mesh) { meshComponent = mesh; }

	FORCEINLINE USphereComponent* GetSphereComponent()const { return sphereComponent; }
	FORCEINLINE void SetSphereComponent(USphereComponent* sphere) { sphereComponent = sphere; }

	//Setting Up Camera
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UCameraComponent* cameraComponent;
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USpringArmComponent* springarmComponent;


	FORCEINLINE UCameraComponent* getCameraComponent()const { return cameraComponent; }
	FORCEINLINE void SetCameraComponent(UCameraComponent* camera) { cameraComponent = camera; }

	FORCEINLINE USpringArmComponent* GetSprinComponent()const { return springarmComponent; }
	FORCEINLINE void SetSpringArmComponent(USpringArmComponent* springarm) { springarmComponent = springarm; }


private:
	void moveForward(float input);
	void moveRight(float input);



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	
};
