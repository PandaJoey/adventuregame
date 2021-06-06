// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ThisSaveGame.generated.h"

//prefix structs with an f
USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Health;
	
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxHealth;
	
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Stamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxStamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int32 Coins;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FVector Location;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FRotator Rotation;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FString WeaponName;
	
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FString LevelName;
};
/**
 * 
 */
UCLASS()
class ADVENTUREGAMEV001_API UThisSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	UThisSaveGame();

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FString PlayerName;
	
	UPROPERTY(VisibleAnywhere, Category = "Basic")
	uint32 UserIndex;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FCharacterStats CharacterStats;

	
	
}
;