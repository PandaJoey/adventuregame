// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Explosive.generated.h"

/*
	Class to add functionality to the explosive item in the game.
*/
UCLASS()
class ADVENTUREGAMEV001_API AExplosive : public AItem
{
	GENERATED_BODY()

public:
	AExplosive();

	// Sets the damage propertie on the explosive.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float Damage;

	// sets the damage type so can be set in blueprints to deal damage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		TSubclassOf<UDamageType> DamageTypeClass;

public:
	/// <summary>
	/// Funciton that deals with overlap begin events.
	/// </summary>
	/// <param name="OverlappedComponent">Check to see what the overlapped component is.</param>
	/// <param name="OtherActor">Check to see if you have overlaped with another actor.</param>
	/// <param name="OtherComp">Checks what the other actors component is that has overlaped.</param>
	/// <param name="OtherBodyIndex">checks otheractors index.</param>
	/// <param name="bFromSweep">Used to see if there is a hit result.</param>
	/// <param name="SweepResult">The result of the hit result.</param>
	virtual void OnOverLapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	/// <summary>
	/// Function that deals with overlap end events
	/// </summary>
	/// <param name="OverlappedComponent">Check to see what the overlapped component is.</param>
	/// <param name="OtherActor">Check to see if you have overlaped with another actor.</param>
	/// <param name="OtherComp">Checks what the other actors component is that has overlaped.</param>
	/// <param name="OtherBodyIndex">checks otheractors index.</param>
	virtual void OnOverLapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

};
