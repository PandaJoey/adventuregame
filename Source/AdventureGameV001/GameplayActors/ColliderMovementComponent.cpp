// Fill out your copyright notice in the Description page of Project Settings.


#include "ColliderMovementComponent.h"

void UColliderMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    //Check if everything is still valid and we are allowed to move
    if(!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime)) 
    {
        return;
    }

    //get and clear the vector from collider
    FVector DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.f) * DeltaTime * 150.f;

    if(!DesiredMovementThisFrame.IsNearlyZero()) 
    {
        FHitResult Hit;
        SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);

        //If we bump into soemthing slide along the side of it
        if(Hit.IsValidBlockingHit())
        {
            //gets hit normal and uses to calcualte how to go in the right direction.
            SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
        }
    }
}

