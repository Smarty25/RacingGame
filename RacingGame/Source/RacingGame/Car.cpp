// Fill out your copyright notice in the Description page of Project Settings.

#include "Car.h"

#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"


// Sets default values
ACar::ACar()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//bReplicates = true;

	CarMovementComponent = CreateDefaultSubobject<UCarMovementComponent>(TEXT("CarMovementComponent"));
	CarReplicationComponent = CreateDefaultSubobject<UCarReplicationComponent>(TEXT("CarReplicationComponent"));
	CarReplicationComponent->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void ACar::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority()) { NetUpdateFrequency = 1; }
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

	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(Role), this, FColor::White, DeltaTime);
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