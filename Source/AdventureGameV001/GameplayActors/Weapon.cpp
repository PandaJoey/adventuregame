// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "MainChar.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Enemy.h"
#include "Engine/SkeletalMeshSocket.h"

AWeapon::AWeapon()
{
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    SkeletalMesh->SetupAttachment(GetRootComponent());

    CombatCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CombatCollision"));
    CombatCollision->SetupAttachment(GetRootComponent());

    bWeaponParticles = false;

    WeaponState = EWeaponState::EWS_Pickup;

    Damage = 25.f;
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();

    CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverLapBegin);
    CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverLapEnd);
    CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AWeapon::OnOverLapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    Super::OnOverLapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    if (WeaponState == EWeaponState::EWS_Pickup && OtherActor)
    {
        AMainChar *MainChar = Cast<AMainChar>(OtherActor);
        if (MainChar)
        {
            MainChar->SetActiveOverlappingItem(this);
        }
    }
}

void AWeapon::OnOverLapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    Super::OnOverLapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
    if (OtherActor)
    {
        AMainChar *MainChar = Cast<AMainChar>(OtherActor);
        if (MainChar)
        {
            MainChar->SetActiveOverlappingItem(nullptr);
        }
    }
}

void AWeapon::Equip(AMainChar *Char)
{
    if (Char)
    {
        SetInstigator(Char->GetController());

        //sets up ignores so the camera doesnt bug out and stops physics
        SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
        SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
        SkeletalMesh->SetSimulatePhysics(false);

        // gets the socket we want to equip a mesh too
        const USkeletalMeshSocket *WeaponSocket = Char->GetMesh()->GetSocketByName("WeaponSocket");

        if (WeaponSocket)
        {
            WeaponSocket->AttachActor(this, Char->GetMesh());
            bRotate = false;
            Char->SetEquippedWeapon(this);
            Char->SetActiveOverlappingItem(nullptr);
        }

        if (OnEquipSound)
            UGameplayStatics::PlaySound2D(this, OnEquipSound);
        if (!bWeaponParticles)
            IdleParticlesComponent->Deactivate();
    }
}

void AWeapon::CombatOnOverLapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if (OtherActor)
    {
        AEnemy *Enemy = Cast<AEnemy>(OtherActor);
        if (Enemy)
        {
            if (Enemy->HitParticles)
            {
                const USkeletalMeshSocket *WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket");
                if (WeaponSocket)
                {
                    FVector SocketLocation = WeaponSocket->GetSocketLocation(SkeletalMesh);
                    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, GetActorLocation(), FRotator(0.f), false);
                }
            }
            if (Enemy->HitTaken)
            {
                UGameplayStatics::PlaySound2D(this, Enemy->HitTaken);
            }
            if(DamageTypeClass)
            {
                UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
            }
            
        }
    }
}
void AWeapon::CombatOnOverLapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
}

void AWeapon::ActivateCollision()
{
    CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    //UGameplayStatics::PlaySound2D(this, SwingSound);
}

void AWeapon::DeactivateCollision()
{
    CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}