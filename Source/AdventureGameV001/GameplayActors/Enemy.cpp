// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "MainChar.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "MainCharPlayerController.h"

// Sets default valuesw
AEnemy::AEnemy()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Sets up the agrosphere to be used in blueprints also attaches it to the root component and sets initial size..
    AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
    AgroSphere->SetupAttachment(GetRootComponent());
    AgroSphere->InitSphereRadius(600.f);

    // Sets up the combatsphere to be used in blueprints also attaches it to the root component and sets initial size.
    CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
    CombatSphere->SetupAttachment(GetRootComponent());
    CombatSphere->InitSphereRadius(75.f);

    // Sets up the box used for the enemy left weapon. attaches it to a socket created on the skeleton in unreal.
    LeftArmCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftArmCollision"));
    LeftArmCollision->SetupAttachment(GetMesh(), FName("EnemyLeftSocket"));

    // Sets up the box used for the enemy Right weapon. attaches it to a socket created on the skeleton in unreal.
    RightArmCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightArmCollision"));
    RightArmCollision->SetupAttachment(GetMesh(), FName("EnemyRightSocket"));

    // Checks to see if the combatsphere is overlapping.
    bOverlappingCombatSphere = false;

    // Sets Enemy Stats
    Health = 100.f;
    Maxhealth = 100.f;
    Damage = 10.f;

    // Sets up enemy attack information
    bAttacking = false;
    AttackMinTime = .5f;
    AttackMaxTime = 3.5f;

    // Sets enemy movmeent status to idle
    EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;

    // Sets how long the delay should be for the enemy to die.
    DeathDelay = 0.5f;

    // Used to see if there is a valid target.
    bHasValidTarget = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
    Super::BeginPlay();

    // Gets the AIController that is to be used for the enemy as the game begins this allows the enemy to interact and move.
    AIController = Cast<AAIController>(GetController());

    // All of these calls are to set up the varios overlap functions that can happen to the enemy when the game begins.
    AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverLapBegin);
    AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverLapEnd);

    CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverLapBegin);
    CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverLapEnd);

    LeftArmCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatBoxOnOverLapBegin);
    LeftArmCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatBoxOnOverLapEnd);

    RightArmCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatBoxOnOverLapBegin);
    RightArmCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatBoxOnOverLapEnd);

    // Sets up collison channels for the enemy weapons so they only interact with the enemy combat sphere.
    LeftArmCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    LeftArmCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    LeftArmCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    LeftArmCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

    RightArmCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RightArmCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    RightArmCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    RightArmCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

    // stops the camera bugging out when colliding with
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}


/// <summary>
/// Called to bind functionality to input that is set in unreal settings.
/// </summary>
/// <param name="PlayerInputComponent">Takes in the PlayerInputComponent</param>
void AEnemy::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}



/// <summary>
/* Function to see if the player has overlapped with the enemy agrosphere, check header for parameter definitions.
   Checks to see if the player has overlapped and if the enemy is alive, then casts to main to say this is what it
   is overlapping with, then calls the move to target function to make it move to the player.
*/
/// </summary>
/// <param name="OverlappedComponent">Check to see what the overlapped component is.</param>
/// <param name="OtherActor">Check to see if you have overlaped with another actor.</param>
/// <param name="OtherComp">Checks what the other actors component is that has overlaped.</param>
/// <param name="OtherBodyIndex">checks otheractors index.</param>
/// <param name="bFromSweep">Used to see if there is a hit result.</param>
/// <param name="SweepResult">The result of the hit result.</param>
void AEnemy::AgroSphereOnOverLapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if (OtherActor && IsAlive())
    {
        AMainChar *MainChar = Cast<AMainChar>(OtherActor);
        if (MainChar)
        {
            MoveToTarget(MainChar);
        }
    }
}



/// <summary>
/*  Function to see if the player has stopped overlapping with the enemy, see header for parameter definitions.
    Checks to see if the other actor is true, then casts to main to say its a main char, then checks to see if it is a main char,
    if it sets valid target to false to say its left, then checks to see if the combat target is main, and then sets it to nullptr to
    remove main from the valid target list. then updates the combat target sets the movement status to idle and tells the ai controller
    to stop moving.
*/
/// </summary>
/// <param name="OverlappedComponent">Check to see what the overlapped component is.</param>
/// <param name="OtherActor">Check to see if you have overlaped with another actor.</param>
/// <param name="OtherComp">Checks what the other actors component is that has overlaped.</param>
/// <param name="OtherBodyIndex">checks otheractors index.</param>
void AEnemy::AgroSphereOnOverLapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor)
    {
        AMainChar *MainChar = Cast<AMainChar>(OtherActor);
        if (MainChar)
        {
            bHasValidTarget = false;
            if (MainChar->CombatTarget == this)
            {
                MainChar->SetCombatTarget(nullptr);
            }

            MainChar->SetHasCombatTarget(false);
            MainChar->UpdateCombatTarget();

            SetEnemyMovmenetStatus(EEnemyMovementStatus::EMS_Idle);
            if (AIController)
            {
                AIController->StopMovement();
            }
        }
    }
}




/// <summary>
/*
    Function to see if enemy and player combatshperes are overlapping, check header for parameters deffs.
    checks to see if the other actor true and if the enemy is alive, if they are it then casts the other actor
    to main, then checks to see if it is a main char, if it is sets valid target to true, tells the main its target
    is the nemy and sets the target to true, then updates the mains combat target, then sets enemys target to mainchar,
    then says the combatspheres are allowed to overlap, then sets up the enemys attack timerss, and allows the enemy to attack.
*/
/// </summary>
/// <param name="OverlappedComponent">Check to see what the overlapped component is.</param>
/// <param name="OtherActor">Check to see if you have overlaped with another actor.</param>
/// <param name="OtherComp">Checks what the other actors component is that has overlaped.</param>
/// <param name="OtherBodyIndex">checks otheractors index.</param>
/// <param name="bFromSweep">Used to see if there is a hit result.</param>
/// <param name="SweepResult">The result of the hit result.</param>
void AEnemy::CombatSphereOnOverLapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if (OtherActor && IsAlive())
    {
        AMainChar *MainChar = Cast<AMainChar>(OtherActor);
        if (MainChar)
        {
            bHasValidTarget = true;
            MainChar->SetCombatTarget(this);
            MainChar->SetHasCombatTarget(true);

            MainChar->UpdateCombatTarget();

            CombatTarget = MainChar;
            bOverlappingCombatSphere = true;
            float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
            GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
        }
    }
}

/*  

*/

/// <summary>
/*  function to set the combatsphere to false, check header for parameters deffs,
    checks to see if the other actor and the other component are true, if they are it casts the otheractor to be a mainchar
    if the mainchar is true it the sets combatsphere overlaps to false, and starts moving to target again to get back in range,
    then sets the combat target to nullptr, if the mainchars combat target is an enemy it sets the combat target to nullptr, sets
    has target to false, updates the combat target, checks to see if the mainplayercontroller is true, if it is it sets the mesh
    to the othercomp, if this mesh is true it removes the enemy health bars. then clears the attack timer for enemies.
*/
/// </summary>
/// <param name="OverlappedComponent">Check to see what the overlapped component is.</param>
/// <param name="OtherActor">Check to see if you have overlaped with another actor.</param>
/// <param name="OtherComp">Checks what the other actors component is that has overlaped.</param>
/// <param name="OtherBodyIndex">checks otheractors index.</param>
void AEnemy::CombatSphereOnOverLapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherComp)
    {
        AMainChar *MainChar = Cast<AMainChar>(OtherActor);
        if (MainChar)
        {

            bOverlappingCombatSphere = false;
            MoveToTarget(MainChar);
            CombatTarget = nullptr;

            if (MainChar->CombatTarget == this)
            {
                MainChar->SetCombatTarget(nullptr);
                MainChar->bHasCombatTarget = false;
                MainChar->UpdateCombatTarget();
            }
            if (MainChar->MainCharPlayerController)
            {
                USkeletalMeshComponent *MainMesh = Cast<USkeletalMeshComponent>(OtherComp);
                if (MainMesh)
                    MainChar->MainCharPlayerController->RemoveEnemyHealthBar();
            }

            GetWorldTimerManager().ClearTimer(AttackTimer);
            //SetEnemyMovmenetStatus(EEnemyMovementStatus::EMS_MoveToTarget);
            //
        }
    }
}



/// <summary>
/// Function to allow enemy to move when player enters agrosphere. first sets the enemymovement status to move to target,
/// creates a moverequest then sets the moverequest to the targetand sets acceptance radius of the moverequest.creates
/// a navepath which is how the enemy will navigate around objects, then calls the moveto function on the aicontroller
/// passing in the movereqeustand the navpath result.
/// </summary>
/// <param name="Target">Check to see if the target is the main character.</param>
void AEnemy::MoveToTarget(AMainChar *Target)
{
    SetEnemyMovmenetStatus(EEnemyMovementStatus::EMS_MoveToTarget);
    if (AIController)
    {
        FAIMoveRequest MoveRequest;
        MoveRequest.SetGoalActor(Target);
        MoveRequest.SetAcceptanceRadius(10.f);

        FNavPathSharedPtr NavPath;

        //&NavPath is an out parameter meaning it takes info out that can be used.
        AIController->MoveTo(MoveRequest, &NavPath);

        // used to show enemy paths/
        /*TArray<FNavPathPoint> PathPoints = NavPath->GetPathPoints();

        for (auto Point : PathPoints)
        {
            FVector Location = Point.Location;

            UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Red, 10.f, 1.5f);
        }
        */
    }
}


/// <summary>
/// Funciton to allow combatboxes to overlap, see header for parameters, if otheractor is true, sets otheractor to main,
/// if the mainchar has hitparticles, sets up weapon sockets
/// </summary>
/// <param name="OverlappedComponent">Check to see what the overlapped component is.</param>
/// <param name="OtherActor">Check to see if you have overlaped with another actor.</param>
/// <param name="OtherComp">Checks what the other actors component is that has overlaped.</param>
/// <param name="OtherBodyIndex">checks otheractors index.</param>
/// <param name="bFromSweep">Used to see if there is a hit result.</param>
/// <param name="SweepResult">The result of the hit result.</param>
void AEnemy::CombatBoxOnOverLapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if (OtherActor)
    {
        AMainChar *MainChar = Cast<AMainChar>(OtherActor);
        if (MainChar)
        {
            if (MainChar->HitParticles)
            {
                const USkeletalMeshSocket *RightParticleSocket = GetMesh()->GetSocketByName("RightParticleSocket");
                const USkeletalMeshSocket *LeftParticleSocket = GetMesh()->GetSocketByName("LeftParticleSocket");
                if (LeftParticleSocket)
                {
                    FVector SocketLocation = LeftParticleSocket->GetSocketLocation(GetMesh());
                    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MainChar->HitParticles, GetActorLocation(), FRotator(0.f), false);
                }
                else if (RightParticleSocket)
                {
                    FVector SocketLocation = RightParticleSocket->GetSocketLocation(GetMesh());
                    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MainChar->HitParticles, GetActorLocation(), FRotator(0.f), false);
                }
            }
            if (MainChar->HitTaken)
            {
                UGameplayStatics::PlaySound2D(this, MainChar->HitTaken);
            }
            //used to give damage to main char
            if (DamageTypeClass)
            {
                UGameplayStatics::ApplyDamage(MainChar, Damage, AIController, this, DamageTypeClass);
            }
        }
    }
}

/// <summary>
/// Function to check if overlapping has ended between the player and the enemys combatbox's.
/// </summary>
/// <param name="OverlappedComponent">Check to see what the overlapped component is.</param>
/// <param name="OtherActor">Check to see if you have overlaped with another actor.</param>
/// <param name="OtherComp">Checks what the other actors component is that has overlaped.</param>
/// <param name="OtherBodyIndex">checks otheractors index.</param>
void AEnemy::CombatBoxOnOverLapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
}

/// <summary>
/// Function to activate the enemies first attack, sets up collisions so it only interacts with the player
/// </summary>
void AEnemy::ActivateAttackOne()
{
    LeftArmCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

/// <summary>
/// Function to deactivate the enemies first attack, stops the collision from happening.
/// </summary>
void AEnemy::DeactivateAttackOne()
{
    LeftArmCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

/// <summary>
/// Function to activate the enemies second attack, sets up collisions so it only interacts with the player
/// </summary>
void AEnemy::ActivateAttackTwo()
{
    RightArmCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

/// <summary>
/// Function to deactivate the enemies second attack, stops the collision from happening.
/// </summary>
void AEnemy::DeactivateAttackTwo()
{
    RightArmCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

/// <summary>
/// Function to activate the enemies third attack, sets up collisions so it only interacts with the player
/// </summary>
void AEnemy::ActivateAttackThree()
{
    LeftArmCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    RightArmCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

/// <summary>
/// Function to deactivate the enemies third attack, stops the collision from happening.
/// </summary>
void AEnemy::DeactivateAttackThree()
{
    LeftArmCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RightArmCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

/// <summary>
/* Allows the enemy to attack, checks to see if it is alive and has a valid target first.
   then checks to see if there is a valid AI controller, if there is it stops the enemy movement,
   then allows the enemy to attack. if not attacking sets attacking to true, calls the anim instance
   then checks to see if there is a valid anim instance and combat montage, then goes through the switch
   to randomise attacks from the enemy, and sets the speed each one will play. then applies the swing sound.
*/
/// </summary>
void AEnemy::Attack()
{
    if (IsAlive() && bHasValidTarget)
    {
        if (AIController)
        {
            AIController->StopMovement();
            SetEnemyMovmenetStatus(EEnemyMovementStatus::EMS_Attacking);
        }
        if (!bAttacking)
        {
            bAttacking = true;
            class UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();
            if (AnimInstance && CombatMontage)
            {
                int32 Section = FMath::RandRange(0, 2);
                switch (Section)
                {
                case 0:
                    AnimInstance->Montage_Play(CombatMontage, 2.2f);
                    AnimInstance->Montage_JumpToSection("Attack_1", CombatMontage);
                    break;
                case 1:
                    AnimInstance->Montage_Play(CombatMontage, 2.2f);
                    AnimInstance->Montage_JumpToSection("Attack_2", CombatMontage);
                    break;
                case 2:
                    AnimInstance->Montage_Play(CombatMontage, 2.2f);
                    AnimInstance->Montage_JumpToSection("Attack_3", CombatMontage);
                    break;
                default:;
                }
            }
            if (SwingSound)
            {
                UGameplayStatics::PlaySound2D(this, SwingSound);
            }
        }
    }
}

/// <summary>
/// Stops the enemy from attacking, sets attacking to false, checks to see if the combatspheres are still overlapping,
/// if they are attacking can continue.
/// </summary>
void AEnemy::AttackEnd()
{
    bAttacking = false;
    if (bOverlappingCombatSphere)
    {
        float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
        GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
    }
}


/// <summary>
/// Function used to take damage.
/// </summary>
/// <param name="DamageAmount">Determines how much damage to take</param>
/// <param name="DamageEvent">Determines if ia damage event has occured.</param>
/// <param name="EventInstigator">Checks to see what actor instigated the event.</param>
/// <param name="DamageCauser">Checks to see which actor caused the damage.</param>
/// <returns></returns>
float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser)
{
    if (Health - DamageAmount <= 0.f)
    {
        Health -= DamageAmount;
        Die(DamageCauser);
    }
    else
    {
        Health -= DamageAmount;
    }
    return DamageAmount;
}

/// <summary>
/* Allows the enemy to die checks to see if there is a animinstance and combatmontage, if there is
   it plays the death animation and sets the movement status to dead. After this it sets all the components
   to not collide so the player can walk through it without issues, then stops all movement, then updates
   the combat target of the player.
*/
/// </summary>
/// <param name="Causer">used to see which actor caused the enemy to die.</param>
void AEnemy::Die(AActor *Causer)
{

    class UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && CombatMontage)
    {
        AnimInstance->Montage_Play(CombatMontage);
        AnimInstance->Montage_JumpToSection(FName("Death"));
        SetEnemyMovmenetStatus(EEnemyMovementStatus::EMS_Dead);
    }

    LeftArmCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RightArmCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    AIController->StopMovement();

    AMainChar *MainChar = Cast<AMainChar>(Causer);
    if (MainChar)
    {
        MainChar->UpdateCombatTarget();
    }
}

/// <summary>
/// Function to deal with the enemy after its died, makes the enemy disapear adding a delay.
/// </summary>
void AEnemy::DeathEnd()
{
    GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
    GetMesh()->bPauseAnims = true;
    GetMesh()->bNoSkeletonUpdate = true;
}

/// <summary>
/// Checks to see if the enemy is still alive, used in many functions as a test.
/// </summary>
/// <returns>returns weather the enemy is not dead.</returns>
bool AEnemy::IsAlive()
{
    return GetEnemyMovmenetStatus() != EEnemyMovementStatus::EMS_Dead;
}

/// <summary>
/// Used to destroy the game object.
/// </summary>
void AEnemy::Disappear()
{
    Destroy();
}