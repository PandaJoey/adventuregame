// Fill out your copyright notice in the Description page of Project Settings.

#include "Critter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ACritter::ACritter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Creates a root component.
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	// Creates the skeletal mesh component.
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	// Sets the skeletal mesh component to the root in blueprints.
	MeshComponent->SetupAttachment(GetRootComponent());

	// Creates the camaera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// Sets the camera to the root component
	Camera->SetupAttachment(GetRootComponent());
	// Sets up the camera offsets from the root component.
	Camera->SetRelativeLocation(FVector(-300.f, 0.f, 300.f));
	Camera->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));

	//AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Sets the current velocity.
	CurrentVelocity = FVector(0.f, 0.f, 0.f);
	// Sets the max speed allowed.
	MaxSpeed = 100.f;

}

// Called when the game starts or when spawned
void ACritter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACritter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Gets the actors location on every frame.
	FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
	// Sets the actors location on every frame.
	SetActorLocation(NewLocation);

}

// Called to bind functionality to input
void ACritter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Binds the moveforward function to a key that is named in the engines input settings.
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACritter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACritter::MoveRight);

}

// Move forward function
void ACritter::MoveForward(float Value) 
{
	// Allows the movement in the x direction between 1 and -1 * max speed if 1 move forward if -1 move back
	CurrentVelocity.X = FMath::Clamp(Value, -1.f, 1.f) * MaxSpeed;

}
// Move right function.
void ACritter::MoveRight(float Value) 
{
	// checks the movement in the y direction between 1 and -1 * maxspeed, if 1 move right. if -1 move left
	CurrentVelocity.Y = FMath::Clamp(Value, -1.f, 1.f) * MaxSpeed;
}
