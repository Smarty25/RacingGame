// Fill out your copyright notice in the Description page of Project Settings.

#include "Car.h"
#include "Components/InputComponent.h"

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

	FVector Acceleration = Force / Mass;

	Velocity = Velocity + (Acceleration * DeltaTime);

	float RotationAngle = MaxTurningSpeed * DeltaTime * SteeringThrow;
	FQuat NewRotation(GetActorUpVector(), FMath::DegreesToRadians(RotationAngle));
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
	PlayerInputComponent->BindAxis("MoveForward", this, &ACar::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACar::MoveRight);
}

void ACar::MoveForward(float Value)
{
	Force = GetActorForwardVector() * Value * AccelerationScalar;
}

void ACar::MoveRight(float Value)
{
	SteeringThrow = Value;
}