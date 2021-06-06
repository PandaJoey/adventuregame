// Fill out your copyright notice in the Description page of Project Settings.

#include "MainCharPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMainCharPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (HudOverlayAsset)
    {
        HUDOverlay = CreateWidget<UUserWidget>(this, HudOverlayAsset);
    }

    HUDOverlay->AddToViewport();
    HUDOverlay->SetVisibility(ESlateVisibility::Visible);

    if (WEnemyHealthBar)
    {
        EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
        if (EnemyHealthBar)
        {
            EnemyHealthBar->AddToViewport();
            EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
        }
        FVector2D Alignment(0.f, 0.f);
        EnemyHealthBar->SetAlignmentInViewport(Alignment);
    }

    if (WPauseMenu)
    {
        PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);
        if (PauseMenu)
        {
            PauseMenu->AddToViewport();
            PauseMenu->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void AMainCharPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (EnemyHealthBar)
    {
        FVector2D PositionInViewport;
        //outfunction
        ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);
        PositionInViewport.Y -= 100.f;
        PositionInViewport.X -= 50.f;

        FVector2D SizeInViewport(300.f, 30.f);

        EnemyHealthBar->SetPositionInViewport(PositionInViewport);
        EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
    }
}

void AMainCharPlayerController::DisplayEnemyHealthBar()
{
    if (EnemyHealthBar)
    {
        bEnemyHealthBarVisible = true;
        EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
    }
}

void AMainCharPlayerController::RemoveEnemyHealthBar()
{
    if (EnemyHealthBar)
    {
        bEnemyHealthBarVisible = false;
        EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
    }
}

void AMainCharPlayerController::DisplayPauseMenu_Implementation()
{
    if (PauseMenu)
    {
        bPauseMenuVisible = true;
        PauseMenu->SetVisibility(ESlateVisibility::Visible);

        FInputModeGameAndUI InputModeGameAndUI;
        SetInputMode(InputModeGameAndUI);
        bShowMouseCursor = true;
    }
}

void AMainCharPlayerController::RemovePauseMenu_Implementation()
{
    if (PauseMenu)
    {
        GameModeOnly();
        bShowMouseCursor = false;

        bPauseMenuVisible = false;
    }
}

void AMainCharPlayerController::TogglePauseMenu()
{
    if (bPauseMenuVisible)
    {
        RemovePauseMenu();
    }
    else
    {
        DisplayPauseMenu();
    }
}

void AMainCharPlayerController::GameModeOnly()
{
    FInputModeGameOnly InputModeGameOnly;
    SetInputMode(InputModeGameOnly);
}