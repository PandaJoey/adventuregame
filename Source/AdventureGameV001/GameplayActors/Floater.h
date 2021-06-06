// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Floater.generated.h"

UCLASS()
class ADVENTUREGAMEV001_API AFloater : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloater();


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ActorMeshComponents")
	UStaticMeshComponent* StaticMesh;
	
	//Location used by SetActorLocation() when BeginPlay() is called
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, category = "Floater Variables")
	FVector InitialLocation;

	//Location of the Actor when dragged in from the editor
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, category = "Floater Variables")
	FVector PlacedLocation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, category = "Floater Variables")
	FVector WorldOrigin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Floater Variables")
	FVector InitialDirection;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, category = "Floater Variables")
	FVector InitialForce;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, category = "Floater Variables")
	FVector InitialTorque;


	UPROPERTY(EditDefaultsOnly ,BlueprintReadWrite, category = "Floater Variables")
	bool bInitializeFloaterLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Floater Variables")
	bool bShouldFloat;


private:

	float RunningTime;

	UPROPERTY(EditAnywhere, Category = "Floater Variables | Wave Parameters")
	float Amplitude;
	UPROPERTY(EditAnywhere, Category = "Floater Variables | Wave Parameters")
	float TimeStretch;
	UPROPERTY(EditAnywhere, Category = "Floater Variables | Wave Parameters")
	float HowFarToComeBack;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	
	
};
