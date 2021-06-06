// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

UCLASS()
class ADVENTUREGAMEV001_API AFloorSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloorSwitch();

	/** Overlap Volume for functionality to be triggered */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FloorSwitch")
	class UBoxComponent* TriggerBox;

	/** Switch for character to step on*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FloorSwitch")
	class UStaticMeshComponent* FloorSwitch;

	/** Door to move when floor switch is pressed.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FloorSwitch")
	UStaticMeshComponent* Door;

	//Initial Location for the door
	UPROPERTY(BlueprintReadWrite, Category = "FloorSwitch")
	FVector InitialDoorLocation;
	
	//Initial Location for floor switch
	UPROPERTY(BlueprintReadWrite, Category = "FloorSwitch")
	FVector InitialSwitchLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FloorSwitch")
	float SwitchTime;

	FTimerHandle SwitchHandle;

	bool bCharacterOnSwitch;





protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverLapBegin(UPrimitiveComponent* OverlappedComponent, 
                                AActor* OtherActor, 
                                UPrimitiveComponent* OtherComp, 
                                int32 OtherBodyIndex, 
                                bool bFromSweep, 
                                const FHitResult &SweepResult );


	UFUNCTION()
	void OnOverLapEnd(UPrimitiveComponent* OverlappedComponent, 
                                AActor* OtherActor, 
                                UPrimitiveComponent* OtherComp, 
                                int32 OtherBodyIndex);

	UFUNCTION(BluePrintImplementableEvent, Category = "FloorSwitch")
	void RaiseDoor();
	
	UFUNCTION(BluePrintImplementableEvent, Category = "FloorSwitch")
	void LowerDoor();

	UFUNCTION(BluePrintImplementableEvent, Category = "FloorSwitch")
	void RaiseFloorSwitch();
	
	UFUNCTION(BluePrintImplementableEvent, Category = "FloorSwitch")
	void LowerFloorSwitch();

	UFUNCTION(BlueprintCallable, Category = "FloorSwitch")
	void UpdateDoorLocation(float Z);

	UFUNCTION(BlueprintCallable, Category = "FloorSwitch")
	void UpdateFloorSwitchLocation(float Z);

	void CloseDoor();
};

