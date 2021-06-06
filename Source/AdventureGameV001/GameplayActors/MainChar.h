// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainChar.generated.h"


UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
    EMS_Normal UMETA(DisplayName = "Normal"),
    EMS_Sprinting UMETA(DisplayName = "Sprinting"),
    EMS_Dead UMETA(DisplayName = "Dead"),
    EMS_MAX UMETA(DisplayName = "DefaultMax")

};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
    ESS_Normal UMETA(DisplayName = "Normal"),
    ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
    ESS_Exausted UMETA(DisplayName = "Exausted"),
    ESS_ExaustedRecovering UMETA(DisplayName = "ExaustedRecovering"),
    ESS_MAX UMETA(DisplayName = "DefaultMax")
};


UCLASS()
class ADVENTUREGAMEV001_API AMainChar : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AMainChar();

    TArray<FVector> PickupLocations;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
    class AWeapon* EquippedWeapon;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
    class AItem* ActiveOverlappingItem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    class USoundCue* HitTaken;

    /******************
     * Camera Setup
     ******************/
    /** meta allow private access only allows use inside the blueprint not outside
       Camera boom is used to position the camera behind the player.
    */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    // Follow Camera
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;

    // Base turn rate to scale turning fucntions for camera
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    float BaseTurnRate;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    float BaseLookUpRate;

    
    /******************
     * Player Stats 
     ******************/

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
    EMovementStatus MovementStatus;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
    EStaminaStatus StaminaStatus;

    UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = "PlayerStats")
    float MaxHealth;
    
    UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "PlayerStats")
    float Health;
    
    UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = "PlayerStats")
    float MaxStamina;
    
    UPROPERTY(EditAnywhere,BluePrintReadWrite, Category = "PlayerStats")
    float Stamina;

    UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "PlayerStats")
    int32 Coins;

    UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "PlayerStats")
    float RunningSpeed;

    UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "PlayerStats")
    float SprintingSpeed;

 
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animations")
    bool bAttacking;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
    class UAnimMontage* CombatMontage;

    UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Movement")
    float StaminaDrainRate;

    UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Movement")
    float MinSprintStamina;

    bool bShiftKeyDown;
    bool bRKeyDown;
    bool bLeftMouseDown;
    bool bMovingForward;
    bool bMovingRight;
    

 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    class UParticleSystem* HitParticles;

    float InterpSpeed;
    bool bInterpToEnemy;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    class AEnemy* CombatTarget;

    UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Combat")
    bool bHasCombatTarget;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
    FVector CombatTargetLocation;

    UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Controller")
    class AMainCharPlayerController* MainCharPlayerController;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TSubclassOf<AEnemy> EnemyFilter;

    UPROPERTY(EditDefaultsOnly, Category = "SaveData")
    TSubclassOf<class AItemStorage> WeaponStorage;



protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

     virtual void Jump() override;

public: 
    // Called every frame
    virtual void Tick(float DeltaTime) override;


    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    
    void AttackKeyDown();
    void AttackKeyUp();
    void PickupKeyDown();
    void PickupKeyUp();
    void Attack();

    void SetInterpToEnemy(bool Interp);

    //i am not going to use this i dont like his idea
    UFUNCTION(BlueprintCallable)
    void AttackEnd();
    
    bool CanMove(float Value);
    // Called for forward/backwards movement inputs;
    void MoveForward(float Value);

    // Called for side to side input
    void MoveRight(float Value);

    // yaw rotation
    void Turn(float Value);
    // pitch rotation
    void LookUp(float Value);

    /* Called via input to turn at a given rate.
       @param Rate this is a normalized rate, i.e 1.0 means 100% of desired turn rate.
    */
    void TurnRate(float Rate);
    
    /* Called via input to turn at a given rate.
       @param Rate this is a normalized rate, i.e 1.0 means 100% of desired up/down rate.
    */
    void LookUpRate(float Rate);

    void DecrementHealth(float TakeDamage);

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    void Die();

    UFUNCTION(BlueprintCallable)
    void IncrementCoins(int32 CoinsToAdd);

    UFUNCTION(BlueprintCallable)
    void IncrementHealth(float HealthToAdd);

    //sets movement status and running speed
    void SetMovementStatus(EMovementStatus Status);
    
    // Pressed down to enable sprinting
    void ShiftKeyDown();

    // Released to stop sprinting;
    void ShiftKeyUp();

    void StaminaStatusSwitch(float DeltaTime);

    UFUNCTION(BlueprintCallable)
    void ShowPickupLocations();

    void SetEquippedWeapon(AWeapon* WeaponToSet);

    UFUNCTION(BlueprintCallable)
    void PlaySwingSound();

    FRotator GetLookAtRotationYaw(FVector Target);

    UFUNCTION(BlueprintCallable)
    void DeathEnd();

    void UpdateCombatTarget();
   
    void SwitchLevel(FName LevelName);

    UFUNCTION(BlueprintCallable)
    void SaveGame();

    UFUNCTION(BlueprintCallable)
    void LoadGame(bool SetPosition);

    UFUNCTION(BlueprintCallable)
    void LoadGameNoSwitch();

    bool bESCDown;
    void ESCDown();
    void ESCUp();

    



    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    FORCEINLINE class UCameraComponent* GetFollowCamara() const { return FollowCamera; }
    FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }
    FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon;}
    FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }
    FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }
    FORCEINLINE void SetHasCombatTarget(bool HasTarget) {bHasCombatTarget = HasTarget; }    

};

