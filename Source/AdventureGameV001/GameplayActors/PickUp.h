// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "PickUp.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTUREGAMEV001_API APickUp : public AItem
{
    GENERATED_BODY()

public:
    APickUp();

    virtual void OnOverLapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) override;

    virtual void OnOverLapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex) override;

    UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
    void OnPickupBP(class AMainChar* Target);
};
