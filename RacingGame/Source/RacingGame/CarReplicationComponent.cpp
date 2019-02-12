// Fill out your copyright notice in the Description page of Project Settings.

#include "CarReplicationComponent.h"
#include "GameFramework/Actor.h"
#include "UnrealNetwork.h"


// Sets default values for this component's properties
UCarReplicationComponent::UCarReplicationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
	// ...
}

void UCarReplicationComponent::GetLifetimeReplicatedProps(TArray < FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCarReplicationComponent, ServerState);
}

// Called when the game starts
void UCarReplicationComponent::BeginPlay()
{
	Super::BeginPlay();

	CarMovementComponent = GetOwner()->FindComponentByClass<UCarMovementComponent>();
	
}


// Called every frame
void UCarReplicationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CarMovementComponent) return;
	FCarMove LastMove = CarMovementComponent->GetLastMove();

	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		UnacknowledgedMoves.Add(LastMove);
		Server_SendMove(LastMove);
	}

	if (GetOwnerRole() == ROLE_Authority && GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
	{
		UpdateServerState(LastMove);
	}

	if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		ClientTick(DeltaTime);
	}
}

void UCarReplicationComponent::ClientTick(float DeltaTime)
{
	if (!CarMovementComponent) return;

	ClientTimeSinceUpdate += DeltaTime;
	if (ClientTimeBetweenlastUpdates < KINDA_SMALL_NUMBER) return;

	FTransform NextTransform;
	float LerpRatio = ClientTimeSinceUpdate / ClientTimeBetweenlastUpdates;

	FVector StartLocation = ClientStartTransform.GetLocation();
	FVector TargetLocation = ServerState.Transform.GetLocation();
	FVector StartDerivative = ClientStartDerivative * ClientTimeBetweenlastUpdates * 100;
	FVector TargetDerivative = ServerState.Velocity * ClientTimeBetweenlastUpdates * 100;

	NextTransform.SetLocation(FMath::CubicInterp(StartLocation, ClientStartDerivative, TargetLocation, TargetDerivative, LerpRatio));
	FVector NextDerivative = FMath::CubicInterpDerivative(StartLocation, ClientStartDerivative, TargetLocation, TargetDerivative, LerpRatio);
	//UE_LOG(LogTemp, Warning, TEXT("NextDerivative = %s"), *NextDerivative.ToString());
	CarMovementComponent->SetVelocity((NextDerivative / (ClientTimeBetweenlastUpdates * 100)));

	FQuat TargetRotation = ServerState.Transform.GetRotation();
	NextTransform.SetRotation(FQuat::Slerp(ClientStartTransform.GetRotation(), TargetRotation, LerpRatio));

	GetOwner()->SetActorTransform(NextTransform);
}

void UCarReplicationComponent::ClearAcknowledgedMoves(FCarMove LastMove)
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

void UCarReplicationComponent::OnRep_ServerState()
{
	if (!CarMovementComponent) return;
	
	switch (GetOwnerRole())
	{
	default:
		break;

	case ROLE_AutonomousProxy:
		AutonomousProxy_OnRep_ServerState();
		break;

	case ROLE_SimulatedProxy:
		SimulatedProxy_OnRep_ServerState();
		break;
	}
}

void UCarReplicationComponent::SimulatedProxy_OnRep_ServerState()
{
	ClientTimeBetweenlastUpdates = ClientTimeSinceUpdate;
	ClientTimeSinceUpdate = 0;
	ClientStartTransform = GetOwner()->GetActorTransform();
	ClientStartDerivative = CarMovementComponent->GetVelocity();
	//UE_LOG(LogTemp, Warning, TEXT("ClientStartDerivative = %s"), *ClientStartDerivative.ToString());

}

void UCarReplicationComponent::AutonomousProxy_OnRep_ServerState()
{
	GetOwner()->SetActorTransform(ServerState.Transform);
	//UE_LOG(LogTemp, Warning, TEXT("ServerState.Velocity = %s"), *ServerState.Velocity.ToString());
	CarMovementComponent->SetVelocity(ServerState.Velocity);

	ClearAcknowledgedMoves(ServerState.LastMove);
	for (const FCarMove& Move : UnacknowledgedMoves)
	{
		CarMovementComponent->SimulateMove(Move);
	}
}

void UCarReplicationComponent::Server_SendMove_Implementation(FCarMove Move)
{
	if (!CarMovementComponent) return;
	CarMovementComponent->SimulateMove(Move);

	UpdateServerState(Move);
}

void UCarReplicationComponent::UpdateServerState(FCarMove Move)
{
	if (!CarMovementComponent) return;
	ServerState.LastMove = Move;
	ServerState.Transform = GetOwner()->GetActorTransform();
	ServerState.Velocity = CarMovementComponent->GetVelocity();
}

bool UCarReplicationComponent::Server_SendMove_Validate(FCarMove Move)
{
	//TODO validate move
	return true;
}