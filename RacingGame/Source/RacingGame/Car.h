// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CarMovementComponent.h"
#include "Car.generated.h"

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

UCLASS()
class RACINGGAME_API ACar : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditAnywhere)
	class UCarMovementComponent* CarMovementComponent;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FCarMove Move);

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FCarState ServerState;

	void Client_MoveForward(float Value);
	void Client_MoveRight(float Value);

	UFUNCTION()
	void OnRep_ServerState();

	TArray<FCarMove> UnacknowledgedMoves;

	void ClearAcknowledgedMoves(FCarMove Move);
};
