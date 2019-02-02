// Fill out your copyright notice in the Description page of Project Settings.

#include "Car.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "UnrealNetwork.h"
#include "GameFramework/GameStateBase.h"

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

	DOREPLIFETIME(ACar, ServerState);
	//DOREPLIFETIME(ACar, ForwardThrow);
	//DOREPLIFETIME(ACar, SteeringThrow);
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

	if (IsLocallyControlled())
	{
		FCarMove Move;
		Move.DeltaTime = DeltaTime;
		Move.ForwardThrow = ForwardThrow;
		Move.SteeringThrow = SteeringThrow;
		Move.Time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

		if (!HasAuthority())
		{
			UnacknowledgedMoves.Add(Move);
			UE_LOG(LogTemp, Warning, TEXT("UnacknowledgedMoves Count = %d"), UnacknowledgedMoves.Num());
		}

		Server_SendMove(Move);
		SimulateMove(Move);
	}

	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(Role), this, FColor::White, DeltaTime);
}

void ACar::ClearAcknowledgedMoves(FCarMove LastMove)
{
	TArray<FCarMove> NewMoves;

	for (const FCarMove& Move : UnacknowledgedMoves)
	{
		if (Move.Time >= LastMove.Time)
		{
			NewMoves.Add(Move);
		}
	}

	UnacknowledgedMoves = NewMoves;
}

void ACar::CheckCollision(const FVector &NewLocation)
{
	FHitResult HitResult = FHitResult();
	AddActorWorldOffset(NewLocation, true, &HitResult);
	if (HitResult.bBlockingHit) { Velocity = FVector(0); /*UE_LOG(LogTemp, Warning, TEXT("bBlockingHit is true"));*/ }
}

void ACar::CalculateRotation(float DeltaTime, float SteeringThrow)
{
	float RotationAngle = (FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime) / TurningRadius * SteeringThrow;
	FQuat NewRotation(GetActorUpVector(), RotationAngle);
	AddActorLocalRotation(NewRotation);
	Velocity = NewRotation.RotateVector(Velocity);
}

void ACar::CalculateVelocity(float DeltaTime, float ForwardThrow)
{
	FVector Force = GetActorForwardVector() * ForwardThrow * AccelerationScalar;
	FVector DragForce = -Velocity.GetSafeNormal() * FMath::Square(Velocity.Size()) * DragCoefficient;
	float NormalForce = -(GetWorld()->GetGravityZ() / 100 * Mass);
	FVector FrictionForce = -Velocity.GetSafeNormal() * NormalForce * FrictionCoefficient;
	Force = Force + DragForce + FrictionForce;

	FVector Acceleration = Force / Mass;

	Velocity = Velocity + (Acceleration * DeltaTime);
}

void ACar::OnRep_ServerState()
{
	SetActorTransform(ServerState.Transform);
	Velocity = ServerState.Velocity;

	ClearAcknowledgedMoves(ServerState.LastMove);
	for (const FCarMove& Move : UnacknowledgedMoves)
	{
		SimulateMove(Move);
	}
	//UE_LOG(LogTemp, Warning, TEXT("Replicated Transform"));
}

void ACar::SimulateMove(const FCarMove& Move)
{
	float DeltaTime = Move.DeltaTime;

	CalculateVelocity(DeltaTime, Move.ForwardThrow);

	CalculateRotation(DeltaTime, Move.SteeringThrow);

	FVector NewLocation = Velocity * DeltaTime * 100;

	CheckCollision(NewLocation);
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
	Value = FMath::Clamp<float>(Value, -1, 1);
	ForwardThrow = Value;
}

void ACar::Client_MoveRight(float Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Client_MoveRight Value = %f"), Value);
	Value = FMath::Clamp<float>(Value, -1, 1);
	SteeringThrow = Value;
}

void ACar::Server_SendMove_Implementation(FCarMove Move)
{
	SimulateMove(Move);
	
	ServerState.LastMove = Move;
	ServerState.Transform = GetActorTransform();
	ServerState.Velocity = Velocity;
}

bool ACar::Server_SendMove_Validate(FCarMove Move)
{
	//TODO validate move
	return true;
}