// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Collider.generated.h"

UCLASS()
class BASICS_API ACollider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACollider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UCameraComponent* Camera;
	
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USpringArmComponent* springarm;

	UPROPERTY(VisibleAnywhere, Category = "MovementComponent")
	class UColliderMovementComponent* MovementComponent;

	//Getters And Setters
	//StaticMesh
	FORCEINLINE UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
	FORCEINLINE void SetMeshComponent(UStaticMeshComponent* inputMesh) { MeshComponent = inputMesh; }
	//Sphere
	FORCEINLINE USphereComponent* GetSphereComponent() const { return SphereComponent; }
	FORCEINLINE void SetSphereComponent(USphereComponent* inputsphere) { SphereComponent = inputsphere; }
	
	// Camera
	FORCEINLINE UCameraComponent* GetCameraComponent() const { return Camera; }
	FORCEINLINE void SetCameraComponent(UCameraComponent* inputcamera) { Camera = inputcamera; }
	//Springarm
	FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return springarm; }
	FORCEINLINE void SetSpringArmComponent(USpringArmComponent* inputspringarm) { springarm = inputspringarm; }
	//Movement
	virtual UPawnMovementComponent* GetMovementComponent()const override;



	
private:
	//Pawn Movement
	void Forward(float input);
	void Right(float input);

	//Camera Movement (Mouse Movement)
	void PitchCamera(float axisvalue);
	void YawCamera(float axisvalue);

	FVector2D CameraInput;
};
