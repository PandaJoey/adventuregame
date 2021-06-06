// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Enemy.h"

/// <summary>
/* Function to initialise animations, checks to see if there is a pawn actor
   if there isnt it trys to find one, then casts the pawn to the enemy.
*/
/// </summary>
void UEnemyAnimInstance::NativeInitializeAnimation()
{
    if(Pawn == nullptr)
    {
        Pawn = TryGetPawnOwner();
        if(Pawn)
        {
            Enemy = Cast<AEnemy>(Pawn);
        }
    }


}

/// <summary>
/*  Function used to update the animation properties, checks to see if there is a pawn actor,
    if there isnt it trys to get one and then casts to it to the enemy class. then checks if 
    pawn exists, if it does it sets the properties that are to be set to the animation.
*/
/// </summary>
void UEnemyAnimInstance::UpdateAnimationProperties()
{
    if(Pawn == nullptr)
    {
        Pawn = TryGetPawnOwner();
        if(Pawn)
        {
            Enemy = Cast<AEnemy>(Pawn);
        }
    }

    if(Pawn)
    {
        FVector Speed = Pawn->GetVelocity();
        FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
        MovementSpeed = LateralSpeed.Size(); 
    }
}





