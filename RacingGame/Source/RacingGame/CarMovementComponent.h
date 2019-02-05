// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CarMovementComponent.generated.h"

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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RACINGGAME_API UCarMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCarMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FCarMove CreateMove(float DeltaTime);

	void SimulateMove(const FCarMove& Move);

	FVector GetVelocity();
	void SetVelocity(FVector VelocityToSet);

	void SetForwardThrow(float Value);
	void SetSteeringThrow(float Value);

private:
	void CalculateVelocity(float DeltaTime, float ForwardThrow);

	void CalculateRotation(float DeltaTime, float SteeringThrow);

	void CheckCollision(const FVector &NewLocation);

	FVector Velocity;

	float ForwardThrow;

	float SteeringThrow;

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
