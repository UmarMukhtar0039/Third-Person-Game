// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Floater.generated.h"

UCLASS()
class UDEMY_BASICS_API AFloater : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloater();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Actor Mesh Comp")
	UStaticMeshComponent* staticmesh;

	UPROPERTY(VisibleInstanceOnly, Category = "Floater Vectors")
		FVector initialLocation;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
