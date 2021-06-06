// Fill out your copyright notice in the Description page of Project Settings.

#include "Floater.h"
#include "Components/StaticMeshComponent.h"



// Sets default values
AFloater::AFloater()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CustomStaticMesh"));

	InitialLocation = FVector(0.0f);
	PlacedLocation = FVector(0.0f);
	WorldOrigin = FVector(0.0f, 0.0f, 0.0f);
	InitialDirection = FVector(0.0f, 0.0f, 0.0f);

	bInitializeFloaterLocations = false;
	bShouldFloat = false;

	InitialForce = FVector(0.0f, 0.0f, 0.0f);
	InitialTorque = FVector(0.0f, 0.0f, 0.0f);

	RunningTime = 0.0f;

	Amplitude = 1.0f;
	TimeStretch = 1.0f;
	HowFarToComeBack = 1.0f;

}

// Called when the game starts or when spawned
void AFloater::BeginPlay()
{
	Super::BeginPlay();

	float InitialX = FMath::FRandRange(-1.0f, 1.0f);
	float InitialY = FMath::FRandRange(-1.0f, 1.0f);
	float InitialZ = FMath::FRandRange(-1.0f, 1.0f);

	InitialLocation.X = InitialX;
	InitialLocation.Y = InitialY;
	InitialLocation.Z = InitialZ;

	InitialLocation *= 500.0f;

	PlacedLocation = GetActorLocation();

	if(bInitializeFloaterLocations) 
	{
		SetActorLocation(InitialLocation);
	}
	FHitResult HitResult;
	
	/*
	Vector LocalOffset = FVector(200.f, 0.0f, 0.0f);
	FVector WorldOffset = FVector(0.0f, 200.0f, 0.0f);
	AddActorLocalOffset(LocalOffset, true, &HitResult);
	AddActorWorldOffset(LocalOffset, true, &HitResult);
    //rotator is y z x in order, pitch is y yaw is z roll is x
	FRotator LocalRotation = FRotator(30.0f, 0.0f, 0.0f);
	FRotator WorldRotation = FRotator(0.0f, 0.0f, 30.0f);

	AddActorLocalRotation(LocalRotation);
	AddActorWorldRotation(WorldRotation);
	*/

	StaticMesh->AddForce(InitialForce);
	StaticMesh->AddTorque(InitialTorque);

}

// Called every frame
void AFloater::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bShouldFloat)
	{

		FVector NewLocation = GetActorLocation();
		NewLocation.Z = NewLocation.Z + Amplitude * FMath::Sin(TimeStretch * RunningTime) + HowFarToComeBack;
		//NewLocation.X = NewLocation.X + Amplitude * FMath::Sin(TimeStretch * RunningTime) + HowFarToComeBack;
		NewLocation.Y = NewLocation.Y + Amplitude * FMath::Cos(TimeStretch * RunningTime) + HowFarToComeBack;


		SetActorLocation(NewLocation);
		RunningTime += DeltaTime;
		
		/*
		FHitResult HitResult;
		AddActorLocalOffset(InitialDirection, true, &HitResult);
		FVector HitLocation = HitResult.Location;

		//UE_LOG(LogTemp, Warning, TEXT("Hit Location: X: = %f, Y = %f, Z = %f"), HitLocation.X, HitLocation.Y, HitLocation.Z);
		*/
	}

}

