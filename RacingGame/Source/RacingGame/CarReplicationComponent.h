// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CarMovementComponent.h"
#include "CarReplicationComponent.generated.h"

USTRUCT()
struct FCarState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		FCarMove LastMove;

	UPROPERTY()
		FTransform Transform;

	UPROPERTY()
		FVector Velocity;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RACINGGAME_API UCarReplicationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCarReplicationComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
		FCarState ServerState;

	float ClientTimeSinceUpdate;
	float ClientTimeBetweenlastUpdates;
	FVector ClientStartLocation;

	void ClientTick(float DeltaTime);
	
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SendMove(FCarMove Move);

	void UpdateServerState(FCarMove Move);

	UFUNCTION()
	void OnRep_ServerState();
	void SimulatedProxy_OnRep_ServerState();
	void AutonomousProxy_OnRep_ServerState();

	TArray<FCarMove> UnacknowledgedMoves;

	void ClearAcknowledgedMoves(FCarMove Move);
	
	UPROPERTY()
	UCarMovementComponent* CarMovementComponent;
};
