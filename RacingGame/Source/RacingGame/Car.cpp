// Fill out your copyright notice in the Description page of Project Settings.

#include "Car.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "UnrealNetwork.h"

// Sets default values
ACar::ACar()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//bReplicates = true;
}

// Called when the game starts or when spawned
void ACar::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority()) { NetUpdateFrequency = 1; }
}

void ACar::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACar, ReplicatedTransform);
}

FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "ERROR";
	}
}

// Called every frame
void ACar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Force = GetActorForwardVector() * ForwardThrow * AccelerationScalar;

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

	if (HasAuthority())
	{
		ReplicatedTransform = GetActorTransform();
	}

	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(Role), this, FColor::White, DeltaTime);
}

void ACar::OnRep_ReplicatedTransform()
{
	SetActorTransform(ReplicatedTransform);
	//UE_LOG(LogTemp, Warning, TEXT("Replicated Transform"));
}

// Called to bind functionality to input
void ACar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ACar::Client_MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACar::Client_MoveRight);
}

void ACar::Client_MoveForward(float Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Client_MoveForward Value = %f"), Value);
	ForwardThrow = Value;
	Server_MoveForward(Value);
}

void ACar::Client_MoveRight(float Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Client_MoveRight Value = %f"), Value);
	SteeringThrow = Value;
	Server_MoveRight(Value);
}

void ACar::Server_MoveForward_Implementation(float Value)
{
	ForwardThrow = Value;
}

bool ACar::Server_MoveForward_Validate(float Value)
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