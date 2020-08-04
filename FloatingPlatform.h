// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

UCLASS()
class BASICS_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	UStaticMeshComponent* Platform;
	
	UPROPERTY(EditAnywhere)
	FVector StartPoint;
	UPROPERTY(EditAnywhere, Category = FloatingPlatform, meta = (MakeEditWidget = "true"))
	FVector EndPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="platform")
	float InterpSpeed;

	FTimerHandle InterpTimer;

	UPROPERTY(EditAnywhere)
	bool bInterping;

	float InterpTime;
	float Distance;

	void ToggleInterping();

	void SwapVector(FVector& vec1, FVector& vec2);
};
