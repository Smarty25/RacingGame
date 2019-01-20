// Fill out your copyright notice in the Description page of Project Settings.

#include "Car.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"

// Sets default values
ACar::ACar()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACar::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector DragForce = -Velocity.GetSafeNormal() * FMath::Square(Velocity.Size()) * DragCoefficient;
	float NormalForce = -(GetWorld()->GetGravityZ() / 100 * Mass);
	FVector FrictionForce = -Velocity.GetSafeNormal() * NormalForce * FrictionCoefficient;
	Force = Force + DragForce + FrictionForce;

	FVector Acceleration = Force / Mass;
	
	Velocity = Velocity + (Acceleration * DeltaTime);

	float RotationAngle = (FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime) / TurningRadius * SteeringThrow;
	FQuat NewRotation(GetActorUpVector(), RotationAngle);
	AddActorLocalRotation(NewRotation);
	Velocity = NewRotation.RotateVector(Velocity);

	FVector NewLocation = Velocity * DeltaTime * 100;

	FHitResult HitResult = FHitResult();
	AddActorWorldOffset(NewLocation, true, &HitResult);
	if (HitResult.bBlockingHit) { Velocity = FVector(0); /*UE_LOG(LogTemp, Warning, TEXT("bBlockingHit is true"));*/ }

}

// Called to bind functionality to input
void ACar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ACar::Server_MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACar::Server_MoveRight);
}

void ACar::Server_MoveForward_Implementation(float Value)
{
	Force = GetActorForwardVector() * Value * AccelerationScalar;
}

bool ACar::Server_MoveForward_Validate(float Value)
{
	if (FMath::Abs(Value) <= .5)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ACar::Server_MoveRight_Implementation(float Value)
{
	SteeringThrow = Value;
}

bool ACar::Server_MoveRight_Validate(float Value)
{
	if (FMath::Abs(Value) <= 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}