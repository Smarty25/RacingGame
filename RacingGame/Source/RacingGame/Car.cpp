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

	FVector NewLocation = Velocity * DeltaTime * 100;
	AddActorWorldOffset(NewLocation);

}

// Called to bind functionality to input
void ACar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ACar::MoveForward);
}

void ACar::MoveForward(float Value)
{
	Force = GetActorForwardVector() * Value * AccelerationScalar;
}

