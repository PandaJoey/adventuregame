// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Critter.generated.h"


UCLASS()
class ADVENTUREGAMEV001_API ACritter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACritter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	

	// Creates a skeletal mesh that can be used in blueprints to assign to.
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USkeletalMeshComponent* MeshComponent;

	// Creates the camara component can be used in blueprints to assign to.
	UPROPERTY(EditAnywhere, Category = "Cameras")
	class UCameraComponent* Camera;

	// used to set max movement speed of the critter in blueprints can be edited in unreal.
	UPROPERTY(EditAnywhere, Category = "Pawn Movement")
	float MaxSpeed;
private:

	// Functions to allow the critter to move forward and right
	void MoveForward(float Value);
	void MoveRight(float Value);
	
	// allows you to get the currentvelocity.
	FVector CurrentVelocity;
	

	
	
};
