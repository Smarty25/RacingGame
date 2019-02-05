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

	CarMovementComponent = CreateDefaultSubobject<UCarMovementComponent>(TEXT("CarMovementComponent"));
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
		FCarMove Move = CarMovementComponent->CreateMove(DeltaTime);

		if (!HasAuthority())
		{
			UnacknowledgedMoves.Add(Move);
			CarMovementComponent->SimulateMove(Move);
			UE_LOG(LogTemp, Warning, TEXT("UnacknowledgedMoves Count = %d"), UnacknowledgedMoves.Num());
		}

		Server_SendMove(Move);
	}

	if (Role == ROLE_SimulatedProxy)
	{
		CarMovementComponent->SimulateMove(ServerState.LastMove);
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

void ACar::OnRep_ServerState()
{
	SetActorTransform(ServerState.Transform);
	CarMovementComponent->SetVelocity(ServerState.Velocity);

	ClearAcknowledgedMoves(ServerState.LastMove);
	for (const FCarMove& Move : UnacknowledgedMoves)
	{
		CarMovementComponent->SimulateMove(Move);
	}
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
	Value = FMath::Clamp<float>(Value, -1, 1);
	CarMovementComponent->SetForwardThrow(Value);
}

void ACar::Client_MoveRight(float Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Client_MoveRight Value = %f"), Value);
	Value = FMath::Clamp<float>(Value, -1, 1);
	CarMovementComponent->SetSteeringThrow(Value);
}

void ACar::Server_SendMove_Implementation(FCarMove Move)
{
	CarMovementComponent->SimulateMove(Move);
	
	ServerState.LastMove = Move;
	ServerState.Transform = GetActorTransform();
	ServerState.Velocity = CarMovementComponent->GetVelocity();
}

bool ACar::Server_SendMove_Validate(FCarMove Move)
{
	//TODO validate move
	return true;
}