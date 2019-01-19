// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Car.generated.h"

UCLASS()
class RACINGGAME_API ACar : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void MoveForward(float Value);
	void MoveRight(float Value);

	FVector Velocity;
	FVector Force;
	float SteeringThrow;

	UPROPERTY(EditAnywhere)
	float Mass = 1000;

	UPROPERTY(EditAnywhere)
	float AccelerationScalar = 1000;

	UPROPERTY(EditAnywhere)
	float DragCoefficient = 2;

	UPROPERTY(EditAnywhere)
	float FrictionCoefficient = 1;
	
	UPROPERTY(EditAnywhere)
	float TurningRadius = 90;
};
