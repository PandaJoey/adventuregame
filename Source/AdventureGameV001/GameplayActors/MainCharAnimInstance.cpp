// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainChar.h"


void UMainCharAnimInstance::NativeInitializeAnimation()
{
    if(Pawn == nullptr)
    {
        Pawn = TryGetPawnOwner();
    }


}

void UMainCharAnimInstance::UpdateAnimationProperties()
{
    if(Pawn == nullptr)
    {
        Pawn = TryGetPawnOwner();
        if(Pawn)
        {
            MainChar = Cast<AMainChar>(Pawn);
        }
    }

    if(Pawn)
    {
        FVector Speed = Pawn->GetVelocity();
        FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
        MovementSpeed = LateralSpeed.Size();

        bIsInAir = Pawn->GetMovementComponent()->IsFalling();
        
        if(MainChar == nullptr)
        {
            MainChar = Cast<AMainChar>(Pawn);
        }
    }
}