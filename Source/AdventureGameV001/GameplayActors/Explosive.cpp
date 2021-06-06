// Fill out your copyright notice in the Description page of Project Settings.

#include "Explosive.h"
#include "MainChar.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "Enemy.h"

/// <summary>
/// Constructor that sets the damage of the explosive.
/// </summary>
/// <returns></returns>
AExplosive::AExplosive()
{
    Damage = 15.f;
}

/// <summary>
/* Funciton that deals with overlap begin events, Overrights the inherited overlap begin function. then checks to
   to see if there is an otheractor, if there is it casts it to main, and then casts it to enemy as it can damage both.
   then checks to see if the otherlap came from a mainchar or enemy, checks to see if there are overlap particles,
   if there is it then spawns a particle immited at the given location. Then checks to see if there is an overlap
   sound, if there is it plays the sound that was set in blueprints. then destroys the items.
*/
/// </summary>
/// <param name="OverlappedComponent">Check to see what the overlapped component is.</param>
/// <param name="OtherActor">Check to see if you have overlaped with another actor.</param>
/// <param name="OtherComp">Checks what the other actors component is that has overlaped.</param>
/// <param name="OtherBodyIndex">checks otheractors index.</param>
/// <param name="bFromSweep">Used to see if there is a hit result.</param>
/// <param name="SweepResult">The result of the hit result.</param>
void AExplosive::OnOverLapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    Super::OnOverLapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    if (OtherActor)
    {
        //used to see if other actior is not a main it will return null
        AMainChar *MainChar = Cast<AMainChar>(OtherActor);
        AEnemy* Enemy = Cast<AEnemy>(OtherActor);
        if (MainChar || Enemy)
        {
            if (OverlapParticles)
            {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);
            }

            if (OverlapSound)
            {
                UGameplayStatics::PlaySound2D(this, OverlapSound);
            }
            UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, DamageTypeClass);
            

            Destroy();
        }
    }
}

/// <summary>
/// function to check if an overlap has ended.
/// </summary>
/// <param name="OverlappedComponent">Check to see what the overlapped component is.</param>
/// <param name="OtherActor">Check to see if you have overlaped with another actor.</param>
/// <param name="OtherComp">Checks what the other actors component is that has overlaped.</param>
/// <param name="OtherBodyIndex">checks otheractors index.</param>
void AExplosive::OnOverLapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    Super::OnOverLapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}