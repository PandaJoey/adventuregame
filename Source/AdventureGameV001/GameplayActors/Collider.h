// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Collider.generated.h"

UCLASS()
class ADVENTUREGAMEV001_API ACollider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACollider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere, Category = "Cameras")
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, Category = "Cameras")
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category = "Movement Component")
	class UColliderMovementComponent* OurMovementComponent;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	//makes function inline and paste it in where called.
	FORCEINLINE UStaticMeshComponent* GetMeshComponent() { return MeshComponent;}
	FORCEINLINE void SetMeshComponent(UStaticMeshComponent* Mesh){ MeshComponent = Mesh;}
	
	FORCEINLINE USphereComponent* GetSphereComponent() { return SphereComponent;}
	FORCEINLINE void SetSphereComponent(USphereComponent* Sphere){ SphereComponent = Sphere;}

	FORCEINLINE UCameraComponent* GetCameraComponent() { return Camera;}
	FORCEINLINE void SetCameraComponent(UCameraComponent* InCamera){ Camera = InCamera;}
	
	FORCEINLINE USpringArmComponent* GetSpringArmComponent() { return SpringArm;}
	FORCEINLINE void SetSpringArmComponent(USpringArmComponent* InSpringArm){ SpringArm = InSpringArm;}


private:

	void MoveForward(float Input);
	void MoveRight(float Input);
	void CameraPitch(float AxisValue);
	void CameraYaw(float AxisValue);

	FVector CurrentVelocity;
	FVector2D CameraInput;
	
};
