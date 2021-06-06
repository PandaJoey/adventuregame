// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"


/*
    State machine used to set the players animation state, is called when using
    the fucntion SetEnemyMovementStatus().
*/
UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
    EMS_Idle UMETA(DisplayName = "Idle"),
    EMS_MoveToTarget UMETA(DisplayName = "MoveToTarget"),
    EMS_Attacking UMETA(DisplayName = "Attacking"),
    EMS_Dead UMETA(DisplayName = "Dead"),
    EMS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class ADVENTUREGAMEV001_API AEnemy : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AEnemy();

    // Used to set a variable to use the movement state machine.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    EEnemyMovementStatus EnemyMovementStatus;

    // Creates a sphere component to use for enemy agro, can be adjusted in blueprints.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    class USphereComponent* AgroSphere;

    // Creates a sphere to say when to engage in combat, can be edited in blueprints, 
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    USphereComponent* CombatSphere;

    // The AI Controller that is used to allow the enemy to move.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    class AAIController* AIController;

    // boolean to see if combat spheres of player and enemy are overlapping to allow combat.
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
    bool bOverlappingCombatSphere;

    // Used to find the combat target in this case the main character.
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
    AMainChar* CombatTarget;

    // Used to deal with player health things. Editiable in blueprints.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float Health;

    // Enemies Max Health, Editiable in blueprints.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float Maxhealth;

    // Sets the enemies damage output, Editable in blueprints.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float Damage;

    // Creates a particle system so we can add particles in blueprints when things are hit.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    class UParticleSystem* HitParticles;

    // Creates a sound cue so we can add a sound in blueprints.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Sounds")
    class USoundCue* HitTaken;
        
    // Creates a sound cue for a weapon swing sound. also you only hve to forward to declare one of each class.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Sounds")
    USoundCue* SwingSound;

    // Creates a box component that is used to attach to the enemy weapons left hand.
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
    class UBoxComponent* LeftArmCollision;

    //  Creates a box component for the right weapon on the enemy blueprint.
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
    UBoxComponent* RightArmCollision;

    // Creates an animation montage that can be added in blueprints after createing a montage.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    class UAnimMontage* CombatMontage;

    // Creates a bool to see if the enemy can attack or not.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    bool bAttacking;

    // Creates a variable to allow delay timers for attacking and dying.
    FTimerHandle AttackTimer;
    FTimerHandle DeathTimer;

    // Creates a variable to specify the death delay for the death timer.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float DeathDelay;

    // Creates a float to specify the minimum time the enemy can attack after combatspheres overlap
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackMinTime;
   
    // Creates a float to specify the Maximum time the enemy can attack after combatspheres overlap
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackMaxTime;

    /* Creates a TSubClassOf taking in a UDamageType which can be used to specify a damage type in blueprints.
       TSubclassOf is used to pass around classes and types safely between scripts. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TSubclassOf<UDamageType> DamageTypeClass;

    // Creates a bool to know the enemy has a valid target, in this case the player.
    bool bHasValidTarget;


    


protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;
     
    // Function to tell the enemy to move to the target when the player is inside the agrosphere, takes in a target of AMainChar.
    UFUNCTION(BluePrintCallable)
    void MoveToTarget(class AMainChar* Target);

    /* OverlapBegin Function used to tell the enemy when a player is inside the agrosphere.
       Takes in an two UPrimitiveComponents one for the overlappedcomponent and one for the other overlapped component, an actor
       which is the other actor, an int32 to get the otherbodyindex, a boolean bFromSweep which is used to tell the hitresult
       if its true or false, if its true the its a valid full sweep hit result and if its false its only some overlap components will work.
       finally takes in the hit result with the result from the sweep to decide if the overlap can be hit or not.*/
    UFUNCTION()
    virtual void AgroSphereOnOverLapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult );

    /* OverlapEnd Function used to tell the enemy the player has left the agrosphere, has the same parameters as above apart from the sweet
    and hit result stuff isnt requried as its an overlap end.*/
    UFUNCTION()
    virtual void AgroSphereOnOverLapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    // OverlapBegin Function to tell the enemy if its combatshphere has overlaped with the players combatsphere to allow combat. has the same parameters as above.
    UFUNCTION()
    virtual void CombatSphereOnOverLapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult );

    /* OverlapEnd Function to tell the enemy if its combatshphere has stopped overlapping with the players combatsphere to stop combat.
    has the same parameters as above apart from the sweet and hit result stuff isnt requried as its an overlap end.*/
    UFUNCTION()
    virtual void CombatSphereOnOverLapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    // OverlapBegin Function used by the enemy weapons to allow damage to hit the player. has the same paremeters as above begin
    UFUNCTION(BlueprintCallable)
    void CombatBoxOnOverLapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
    
    /* OverlapEnd Function to stop the player taking damage when the weapons have stopped overlapping.
    has the same parameters as above apart from the sweet and hit result stuff isnt requried as its an overlap end.*/
    UFUNCTION(BlueprintCallable)
    void CombatBoxOnOverLapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
    
    /* Function used to allow the enemy to take damage, takes in a damageamount, a struct FDamageEvent, 
    which triggers if damage is allowed to be taken an AController to see who instigated the damage event, 
    an actor to see which actor gave the damage. */
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // Function simply to allow a call to an attack animation in blueprints.
    UFUNCTION(BlueprintCallable)
    void ActivateAttackOne();
    
    // Function simply to Stop a call to an attack animation in blueprints.
    UFUNCTION(BlueprintCallable)
    void DeactivateAttackOne();
    
    // Function simply to allow a call to an attack animation in blueprints.
    UFUNCTION(BlueprintCallable)
    void ActivateAttackTwo();
    
    // Function simply to Stop a call to an attack animation in blueprints.
    UFUNCTION(BlueprintCallable)
    void DeactivateAttackTwo();
    
    // Function simply to allow a call to an attack animation in blueprints.
    UFUNCTION(BlueprintCallable)
    void ActivateAttackThree();
    
    // Function simply to Stop a call to an attack animation in blueprints.
    UFUNCTION(BlueprintCallable)
    void DeactivateAttackThree();

    // Function to allow the Enemy to die, takes an AActor to see who caused the death.
    void Die(AActor* Causer);

    // Function to allow the Enemy to attack.
    void Attack();

    //Function that stops the enemy attack callable in blueprints.
    UFUNCTION(BlueprintCallable)
    void AttackEnd();

    // Function used to stop the death animation from playing.
    UFUNCTION(BlueprintCallable)
    void DeathEnd();


    // Boolean Function just to use as a check to see if the Enemy is alive or not
    bool IsAlive();

    // Function used to make the enemy meshes leave the game space.
    void Disappear();

    // Function to set the enemy movement status mostly used to change animations.
    FORCEINLINE void SetEnemyMovmenetStatus(EEnemyMovementStatus Status) { EnemyMovementStatus = Status; }
    FORCEINLINE EEnemyMovementStatus GetEnemyMovmenetStatus() { return EnemyMovementStatus; }
};
