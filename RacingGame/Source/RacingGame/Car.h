// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "CarReplicationComponent.h"
#include "Car.generated.h"

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

	UCarMovementComponent* GetCarMovementComponent() { return CarMovementComponent; }

private:
	UPROPERTY(VisibleAnywhere)
	class UCarMovementComponent* CarMovementComponent;
	UPROPERTY(VisibleAnywhere)
	class UCarReplicationComponent* CarReplicationComponent;

	void Client_MoveForward(float Value);
	void Client_MoveRight(float Value);
};
