// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

/*
	Class that adds functionality to the floating platform actor.
*/
UCLASS()
class ADVENTUREGAMEV001_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();

	// Mesh for the floating platform
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform")
	class UStaticMeshComponent* Mesh;

	// Adds a start point to the platform, travels between this and the end point.
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"), Category = "Platform")
	FVector StartPoint;

	// Adds an end point to the platform, travels betweent his and the start point.
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"), Category = "Platform")
	FVector EndPoint;
	
	// Sets the interpolation speed.
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"), Category = "Platform")
	float InterpSpeed;
	
	// Sets the interpolation time.
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"), Category = "Platform")
	float InterpTime;

	// Sets the interpolation timer.
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"), Category = "Platform")
	FTimerHandle InterpTimer;


	// Sets if the platform is interping or not.
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"), Category = "Platform")
	bool bInterping;

	// sets the distance to travel.
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"), Category = "Platform")
	float Distance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function to toggle between interping and not interping.
	void ToggleInterping();

	// function to swap between two vectors.
	void SwapVectors(FVector& VecOne, FVector& VecTwo);

	
	
};
