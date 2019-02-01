// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Car.generated.h"

USTRUCT()
struct FCarMove
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float ForwardThrow;

	UPROPERTY()
	float SteeringThrow;

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	float Time;
};

USTRUCT()
struct FCarState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FCarMove LastMove;

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	FVector Velocity;
};

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

	void CheckCollision(const FVector &NewLocation);

	void CalculateRotation(float DeltaTime, float SteeringThrow);

	void CalculateVelocity(float DeltaTime, float ForwardThrow);

	void SimulateMove(FCarMove Move);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FCarMove Move);

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FCarState ServerState;

	FVector Velocity;

	void Client_MoveForward(float Value);
	void Client_MoveRight(float Value);

	UFUNCTION()
	void OnRep_ServerState();

	float ForwardThrow;

	float SteeringThrow;

	TArray<FCarMove> UnacknowledgedMoves;

	void ClearAcknowledgedMoves(FCarMove Move);

	//Car Properties
	UPROPERTY(EditAnywhere)
	float Mass = 1000;

	UPROPERTY(EditAnywhere)
	float AccelerationScalar = 10000;

	UPROPERTY(EditAnywhere)
	float DragCoefficient = 2;

	UPROPERTY(EditAnywhere)
	float FrictionCoefficient = .3;
	
	UPROPERTY(EditAnywhere)
	float TurningRadius = 10;
};
