// Fill out your copyright notice in the Description page of Project Settings.

#include "PickUp.h"
#include "MainChar.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"

APickUp::APickUp()
{
}

void APickUp::OnOverLapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    Super::OnOverLapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    if (OtherActor)
    {
        //used to see if other actior is not a main it will return null
        AMainChar *MainChar = Cast<AMainChar>(OtherActor);
        if (MainChar)
        {
            OnPickupBP(MainChar);
            MainChar->PickupLocations.Add(GetActorLocation());
            if (OverlapParticles)
            {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);
            }

            if (OverlapSound)
            {
                UGameplayStatics::PlaySound2D(this, OverlapSound);
            }
            Destroy();
        }
    }
}

void APickUp::OnOverLapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    Super::OnOverLapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
