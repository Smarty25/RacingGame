// Fill out your copyright notice in the Description page of Project Settings.

#include "CarMovementComponent.h"

#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"


// Sets default values for this component's properties
UCarMovementComponent::UCarMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCarMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCarMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FCarMove UCarMovementComponent::CreateMove(float DeltaTime)
{
	FCarMove Move;
	Move.DeltaTime = DeltaTime;
	Move.ForwardThrow = ForwardThrow;
	Move.SteeringThrow = SteeringThrow;
	Move.Time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	return Move;
}

void UCarMovementComponent::SimulateMove(const FCarMove& Move)
{
	float DeltaTime = Move.DeltaTime;

	CalculateVelocity(DeltaTime, Move.ForwardThrow);

	CalculateRotation(DeltaTime, Move.SteeringThrow);

	FVector NewLocation = Velocity * DeltaTime * 100;

	CheckCollision(NewLocation);
}

void UCarMovementComponent::CalculateVelocity(float DeltaTime, float ForwardThrow)
{
	FVector Force = GetOwner()->GetActorForwardVector() * ForwardThrow * AccelerationScalar;
	FVector DragForce = -Velocity.GetSafeNormal() * FMath::Square(Velocity.Size()) * DragCoefficient;
	float NormalForce = -(GetWorld()->GetGravityZ() / 100 * Mass);
	FVector FrictionForce = -Velocity.GetSafeNormal() * NormalForce * FrictionCoefficient;
	Force = Force + DragForce + FrictionForce;

	FVector Acceleration = Force / Mass;

	Velocity = Velocity + (Acceleration * DeltaTime);
}

void UCarMovementComponent::CalculateRotation(float DeltaTime, float SteeringThrow)
{
	float RotationAngle = (FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity) * DeltaTime) / TurningRadius * SteeringThrow;
	FQuat NewRotation(GetOwner()->GetActorUpVector(), RotationAngle);
	GetOwner()->AddActorLocalRotation(NewRotation);
	Velocity = NewRotation.RotateVector(Velocity);
}

void UCarMovementComponent::CheckCollision(const FVector &NewLocation)
{
	FHitResult HitResult = FHitResult();
	GetOwner()->AddActorWorldOffset(NewLocation, true, &HitResult);
	if (HitResult.bBlockingHit) { Velocity = FVector(0); /*UE_LOG(LogTemp, Warning, TEXT("bBlockingHit is true"));*/ }
}

FVector UCarMovementComponent::GetVelocity()
{
	return Velocity;
}

void UCarMovementComponent::SetVelocity(FVector VelocityToSet)
{
	Velocity = VelocityToSet;
}

void UCarMovementComponent::SetForwardThrow(float Value)
{
	ForwardThrow = Value;
}

void UCarMovementComponent::SetSteeringThrow(float Value)
{
	SteeringThrow = Value;
}