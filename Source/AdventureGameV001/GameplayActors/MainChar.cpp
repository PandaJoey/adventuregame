// Fill out your copyright notice in the Description page of Project Settings.

#include "MainChar.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "MainCharPlayerController.h"
#include "ThisSaveGame.h"
#include "ItemStorage.h"

// Sets default values
AMainChar::AMainChar()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    /*
       CameraBoom Inits
       Creates Camera Boom (Pulls Towards Player if Collision Occurs)
       Sets Camera to Follow at specified distance.
       Tells the camera to Rotate arm based on player controller.
    */
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->TargetArmLength = 600.f;
    CameraBoom->bUsePawnControlRotation = true;

    // Set size for collision.
    GetCapsuleComponent()->SetCapsuleSize(48.f, 105.f);

    /*
      FollowCamera Init
      Creates the follow camera, then sets it to be attached to the boom with a socket
      then says its rotation is false so it follows the controller rotation.
    */
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    //Sets turn rates for inputs
    BaseTurnRate = 65.f;
    BaseLookUpRate = 65.f;

    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    // Configure character movement, moves in direction or input at rotation rate below.
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
    GetCharacterMovement()->JumpZVelocity = 650.f;
    GetCharacterMovement()->AirControl = 0.5f;

    MaxHealth = 150.f;
    Health = 50.f;
    MaxStamina = 150.f;
    Stamina = 120.f;
    Coins = 0;

    RunningSpeed = 650.f;
    SprintingSpeed = 1000.f;

    bShiftKeyDown = false;
    bRKeyDown = false;
    bLeftMouseDown = false;

    //Initialise Enums
    MovementStatus = EMovementStatus::EMS_Normal;
    StaminaStatus = EStaminaStatus::ESS_Normal;

    StaminaDrainRate = 25.f;
    MinSprintStamina = 50.f;

    InterpSpeed = 15.f;
    bInterpToEnemy = false;

    bHasCombatTarget = false;

    bMovingForward = false;
    bMovingRight = false;

    bESCDown = false;
}

// Called when the game starts or when spawned
void AMainChar::BeginPlay()
{
    Super::BeginPlay();
    MainCharPlayerController = Cast<AMainCharPlayerController>(GetController());

    LoadGameNoSwitch();

    if(MainCharPlayerController)
    {
        MainCharPlayerController->GameModeOnly();
    }
}

// Called every frame
void AMainChar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (MovementStatus == EMovementStatus::EMS_Dead)
        return;

    StaminaStatusSwitch(DeltaTime);

    //Sets Actors Interp Rotation for smooth turning.
    //i do not like this will change after course is finished.
    if (bInterpToEnemy && CombatTarget)
    {
        FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
        FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

        SetActorRotation(InterpRotation);
    }

    if (CombatTarget)
    {
        CombatTargetLocation = CombatTarget->GetActorLocation();
        if (MainCharPlayerController)
        {
            MainCharPlayerController->EnemyLocation = CombatTargetLocation;
        }
    }
}

// Called to bind functionality to input
void AMainChar::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainChar::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainChar::ShiftKeyDown);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainChar::ShiftKeyUp);

    PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AMainChar::AttackKeyDown);
    PlayerInputComponent->BindAction("Attack", IE_Released, this, &AMainChar::AttackKeyUp);

    PlayerInputComponent->BindAction("Pickup", IE_Pressed, this, &AMainChar::PickupKeyDown);
    PlayerInputComponent->BindAction("Pickup", IE_Released, this, &AMainChar::PickupKeyUp);

    PlayerInputComponent->BindAxis("MoveForward", this, &AMainChar::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMainChar::MoveRight);

    PlayerInputComponent->BindAxis("Turn", this, &AMainChar::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AMainChar::LookUp);
    PlayerInputComponent->BindAxis("TurnRate", this, &AMainChar::TurnRate);
    PlayerInputComponent->BindAxis("LookUpRate", this, &AMainChar::LookUpRate);

    PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMainChar::ESCDown);
    PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMainChar::ESCUp);
}

bool AMainChar::CanMove(float Value)
{
    if (MainCharPlayerController)
    {
        return (Value != 0.f) &&
               (!bAttacking) &&
               (MovementStatus != EMovementStatus::EMS_Dead) &&
               !MainCharPlayerController->bPauseMenuVisible;
    }

    return false;
}

// Called for forward/backwards movement inputs;
void AMainChar::MoveForward(float Value)
{
    bMovingForward = false;
    if (CanMove(Value))
    {

        // Find out which way is forward.
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator Yawrotation(0.f, Rotation.Yaw, 0.f);

        //gets the x axis of the rotation matrix and sets it to x direction magnitude of 1. x forward y right z up.
        const FVector Direction = FRotationMatrix(Yawrotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
        bMovingForward = true;
    }
}

// Called for side to side input
void AMainChar::MoveRight(float Value)
{
    bMovingRight = false;
    if (CanMove(Value))
    {
        // Find out which way is forward.
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator Yawrotation(0.f, Rotation.Yaw, 0.f);

        //gets the x axis of the rotation matrix and sets it to x direction magnitude of 1. x forward y right z up.
        const FVector Direction = FRotationMatrix(Yawrotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
        bMovingRight = true;
    }
}

void AMainChar::Turn(float Value)
{
    if (CanMove(Value))
    {
        AddControllerYawInput(Value);
    }
}

void AMainChar::LookUp(float Value)
{
    if (CanMove(Value))
    {
        AddControllerPitchInput(Value);
    }
}

void AMainChar::TurnRate(float Rate)
{
    //roates at a smooth constant rate
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}
void AMainChar::LookUpRate(float Rate)
{
    //roates at a smooth constant rate
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainChar::DecrementHealth(float TakeDamage)
{
    if (Health - TakeDamage <= 0.f)
    {
        Die();
    }
    else
    {
        Health -= TakeDamage;
    }
}

void AMainChar::IncrementCoins(int32 CoinsToAdd)
{
    Coins += CoinsToAdd;
}

void AMainChar::IncrementHealth(float HealthToAdd)
{
    if (Health + HealthToAdd >= MaxHealth)
    {
        Health = MaxHealth;
    }
    else
    {
        Health += HealthToAdd;
    }
}

void AMainChar::AMainChar::Die()
{
    if(MovementStatus == EMovementStatus::EMS_Dead) return;
    UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && CombatMontage)
    {
        AnimInstance->Montage_Play(CombatMontage, 1.f);
        AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
    }
    SetMovementStatus(EMovementStatus::EMS_Dead);
}

void AMainChar::DeathEnd()
{
    GetMesh()->bPauseAnims = true;
    GetMesh()->bNoSkeletonUpdate = true;
}

void AMainChar::Jump()
{   
    if (MainCharPlayerController)
            if (MainCharPlayerController->bPauseMenuVisible)
                return;

    if (MovementStatus != EMovementStatus::EMS_Dead)
    {
        
        Super::Jump();
    }
}

void AMainChar::SetMovementStatus(EMovementStatus Status)
{
    MovementStatus = Status;
    if (MovementStatus == EMovementStatus::EMS_Sprinting)
    {
        GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
    }
    else
    {
        GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
    }
}

void AMainChar::AttackKeyDown()
{
    bLeftMouseDown = true;
    if (MovementStatus == EMovementStatus::EMS_Dead)
        return;

    if (MainCharPlayerController)
        if (MainCharPlayerController->bPauseMenuVisible)
            return;

    if (EquippedWeapon)
    {
        Attack();
    }
}

void AMainChar::AttackKeyUp()
{
    bLeftMouseDown = false;
}

void AMainChar::PickupKeyDown()
{
    bRKeyDown = true;
    if (MovementStatus == EMovementStatus::EMS_Dead)
        return;

    if (MainCharPlayerController)
        if (MainCharPlayerController->bPauseMenuVisible)
            return;

    if (ActiveOverlappingItem)
    {
        AWeapon *Weapon = Cast<AWeapon>(ActiveOverlappingItem);
        if (Weapon)
        {
            Weapon->Equip(this);
            SetActiveOverlappingItem(nullptr);
        }
    }
}

void AMainChar::PickupKeyUp()
{
    bRKeyDown = false;
}

void AMainChar::ShiftKeyDown()
{
    bShiftKeyDown = true;
}

void AMainChar::ShiftKeyUp()
{
    bShiftKeyDown = false;
}

void AMainChar::StaminaStatusSwitch(float DeltaTime)
{
    float DeltaStamina = StaminaDrainRate * DeltaTime;

    switch (StaminaStatus)
    {
    case EStaminaStatus::ESS_Normal:
        if (bShiftKeyDown && bMovingRight && bMovingForward)
        {
            if (Stamina - DeltaStamina <= MinSprintStamina)
            {
                SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
                Stamina -= DeltaStamina;
            }
            else
            {
                Stamina -= DeltaStamina;
            }
            if (bMovingForward || bMovingRight)
            {
                SetMovementStatus(EMovementStatus::EMS_Sprinting);
            }
            else
            {
                SetMovementStatus(EMovementStatus::EMS_Normal);
            }
        }
        else // Shift Key Up
        {
            if (Stamina + DeltaStamina >= MaxStamina)
            {
                Stamina = MaxStamina;
            }
            else
            {
                Stamina += DeltaStamina;
            }
            SetMovementStatus(EMovementStatus::EMS_Normal);
        }
        break;
    case EStaminaStatus::ESS_BelowMinimum:
        if (bShiftKeyDown)
        {
            if (Stamina - DeltaStamina <= 0.f)
            {
                SetStaminaStatus(EStaminaStatus::ESS_Exausted);
                Stamina = 0.f;
                SetMovementStatus(EMovementStatus::EMS_Normal);
            }
            else
            {
                Stamina -= DeltaStamina;
                if (bMovingForward || bMovingRight)
                {
                    SetMovementStatus(EMovementStatus::EMS_Sprinting);
                }
                else
                {
                    SetMovementStatus(EMovementStatus::EMS_Normal);
                }
            }
        }
        else // Shift Key Up
        {
            if (Stamina + DeltaStamina >= MinSprintStamina)
            {
                SetStaminaStatus(EStaminaStatus::ESS_Normal);
                Stamina += DeltaStamina;
            }
            else
            {
                Stamina += DeltaStamina;
            }
            SetMovementStatus(EMovementStatus::EMS_Normal);
        }
        break;
    case EStaminaStatus::ESS_Exausted:
        if (bShiftKeyDown)
        {
            Stamina = 0.f;
        }
        else // Shift Key Up
        {
            SetStaminaStatus(EStaminaStatus::ESS_ExaustedRecovering);
            Stamina += DeltaStamina;
        }
        SetMovementStatus(EMovementStatus::EMS_Normal);
        break;
    case EStaminaStatus::ESS_ExaustedRecovering:
        if (Stamina + DeltaStamina >= MinSprintStamina)
        {
            SetStaminaStatus(EStaminaStatus::ESS_Normal);
            Stamina += DeltaStamina;
        }
        else
        {
            Stamina += DeltaStamina;
        }
        SetMovementStatus(EMovementStatus::EMS_Normal);
        break;
    default:
        break;
    }
}

void AMainChar::ShowPickupLocations()
{
    /*standard for
    for (int32 i = 0; i < PickupLocations.Num(); i++)
    {
        UKismetSystemLibrary::DrawDebugSphere(this, PickupLocations[i], 25.f, 8, FLinearColor::Green, 10.f, .5f);
    }
    */

    // for each better choice for this
    for (/*FVector*/ auto Location : PickupLocations)
    {
        UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Green, 10.f, .5f);
    }
}

void AMainChar::SetEquippedWeapon(AWeapon *WeaponToSet)
{
    if (EquippedWeapon)
    {
        EquippedWeapon->Destroy();
    }

    EquippedWeapon = WeaponToSet;
}

void AMainChar::Attack()
{
    if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead)
    {
        bAttacking = true;
        SetInterpToEnemy(true);

        UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance && CombatMontage)
        {
            int32 Section = FMath::RandRange(0, 1);
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
            default:;
            }
        }
        /*
        if (EquippedWeapon->SwingSound)
        {
            UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
        }
        */
    }
}

void AMainChar::AttackEnd()
{
    bAttacking = false;
    SetInterpToEnemy(false);
    if (bLeftMouseDown)
    {
        Attack();
    }
}

void AMainChar::PlaySwingSound()
{
    if (EquippedWeapon->SwingSound)
    {
        UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
    }
}

void AMainChar::SetInterpToEnemy(bool Interp)
{
    bInterpToEnemy = Interp;
}

//i do not like this will change after course is finished.
FRotator AMainChar::GetLookAtRotationYaw(FVector Target)
{
    FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
    FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
    return LookAtRotationYaw;
}

float AMainChar::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser)
{
    if (Health - DamageAmount <= 0.f)
    {
        Health -= DamageAmount;
        Die();
        if (DamageCauser)
        {
            AEnemy *Enemy = Cast<AEnemy>(DamageCauser);
            if (Enemy)
            {
                Enemy->bHasValidTarget = false;
            }
        }
    }
    else
    {
        Health -= DamageAmount;
    }

    return DamageAmount;
}

void AMainChar::UpdateCombatTarget()
{
    TArray<AActor *> OverLappingActors;
    GetOverlappingActors(OverLappingActors, EnemyFilter);

    if (OverLappingActors.Num() == 0)
    {
        if (MainCharPlayerController)
        {
            MainCharPlayerController->RemoveEnemyHealthBar();
        }
        return;
    }

    AEnemy *ClosestEnemy = Cast<AEnemy>(OverLappingActors[0]);

    if (ClosestEnemy)
    {
        FVector Location = GetActorLocation();
        float MinDistance = (ClosestEnemy->GetActorLocation() - Location).Size();

        for (auto Actor : OverLappingActors)
        {
            AEnemy *Enemy = Cast<AEnemy>(Actor);
            if (Enemy)
            {
                float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();
                if (DistanceToActor < MinDistance)
                {
                    MinDistance = DistanceToActor;
                    ClosestEnemy = Enemy;
                }
            }
        }
        if (MainCharPlayerController)
        {
            MainCharPlayerController->DisplayEnemyHealthBar();
        }
        SetCombatTarget(ClosestEnemy);
        bHasCombatTarget = true;
    }
}

void AMainChar::SwitchLevel(FName LevelName)
{
    UWorld *World = GetWorld();
    if (World)
    {
        FString CurrentLevel = World->GetMapName();
        // have to dereference to get the string
        FName CurrentLelveName(*CurrentLevel);
        if (CurrentLelveName != LevelName)
        {
            UE_LOG(LogTemp, Log, TEXT("Transitioning to new level: %s"), *LevelName.ToString());
            UGameplayStatics::OpenLevel(World, LevelName);
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("names match"));
        }
    }
}

void AMainChar::SaveGame()
{
    //casting to actor to create an instance of it to use as a variable
    UThisSaveGame *SaveGameInstance = Cast<UThisSaveGame>(UGameplayStatics::CreateSaveGameObject(UThisSaveGame::StaticClass()));

    SaveGameInstance->CharacterStats.Health = Health;
    SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
    SaveGameInstance->CharacterStats.Stamina = Stamina;
    SaveGameInstance->CharacterStats.MaxStamina = MaxStamina;
    SaveGameInstance->CharacterStats.Coins = Coins;

    FString MapName = GetWorld()->GetMapName();
    MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
    SaveGameInstance->CharacterStats.LevelName = MapName;
    //UE_LOG(LogTemp, Warning, TEXT("MapName : %s"), *MapName);

    if (EquippedWeapon)
    {
        SaveGameInstance->CharacterStats.WeaponName = EquippedWeapon->Name;
    }

    SaveGameInstance->CharacterStats.Location = GetActorLocation();
    SaveGameInstance->CharacterStats.Rotation = GetActorRotation();

    // saves the game to memory
    UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);
}

void AMainChar::LoadGame(bool SetPosition)
{
    //casting to actor to create an instance of it to use as a variable
    UThisSaveGame *LoadGameInstance = Cast<UThisSaveGame>(UGameplayStatics::CreateSaveGameObject(UThisSaveGame::StaticClass()));

    // works from right to left, so saves to this instance of loadgame after its loaded the data.
    LoadGameInstance = Cast<UThisSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

    Health = LoadGameInstance->CharacterStats.Health;
    MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
    Stamina = LoadGameInstance->CharacterStats.Stamina;
    MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
    Coins = LoadGameInstance->CharacterStats.Coins;

    if (WeaponStorage)
    {
        AItemStorage *Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
        if (Weapons)
        {
            FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;

            if (Weapons->WeaponMap.Contains(WeaponName))
            {
                AWeapon *WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
                WeaponToEquip->Equip(this);
            }
        }
    }

    if (SetPosition)
    {
        SetActorLocation(LoadGameInstance->CharacterStats.Location);
        SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
    }

    SetMovementStatus(EMovementStatus::EMS_Normal);
    GetMesh()->bPauseAnims = false;
    GetMesh()->bNoSkeletonUpdate = false;

    if(LoadGameInstance->CharacterStats.LevelName != TEXT(""))
    {
        FName LevelName = (*LoadGameInstance->CharacterStats.LevelName); 
        SwitchLevel(LevelName);
    }
}

void AMainChar::LoadGameNoSwitch()
{   
     //casting to actor to create an instance of it to use as a variable
    UThisSaveGame *LoadGameInstance = Cast<UThisSaveGame>(UGameplayStatics::CreateSaveGameObject(UThisSaveGame::StaticClass()));

    // works from right to left, so saves to this instance of loadgame after its loaded the data.
    LoadGameInstance = Cast<UThisSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

    Health = LoadGameInstance->CharacterStats.Health;
    MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
    Stamina = LoadGameInstance->CharacterStats.Stamina;
    MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
    Coins = LoadGameInstance->CharacterStats.Coins;

    if (WeaponStorage)
    {
        AItemStorage *Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
        if (Weapons)
        {
            FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;

            if (Weapons->WeaponMap.Contains(WeaponName))
            {
                AWeapon *WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
                WeaponToEquip->Equip(this);
            }
        }
    }

    SetMovementStatus(EMovementStatus::EMS_Normal);
    GetMesh()->bPauseAnims = false;
    GetMesh()->bNoSkeletonUpdate = false;
}

void AMainChar::ESCDown()
{
    bESCDown = true;
    if (MainCharPlayerController)
    {
        MainCharPlayerController->TogglePauseMenu();
    }
}
void AMainChar::ESCUp()
{
    bESCDown = false;
}
