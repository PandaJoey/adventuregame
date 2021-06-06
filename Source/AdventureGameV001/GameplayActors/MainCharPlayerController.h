// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainCharPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTUREGAMEV001_API AMainCharPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	// reference to the umg assett in the editor.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> HudOverlayAsset;

	// variable to hold widget after creation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HUDOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WEnemyHealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* EnemyHealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WPauseMenu;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* PauseMenu;

	bool bEnemyHealthBarVisible;
	bool bPauseMenuVisible;

	FVector EnemyLocation;



protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
public:


	void DisplayEnemyHealthBar();

	void RemoveEnemyHealthBar();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void DisplayPauseMenu();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void RemovePauseMenu();
	
	void TogglePauseMenu();

	void GameModeOnly();
};
