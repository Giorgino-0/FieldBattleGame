// Fill out your copyright notice in the Description page of Project Settings.


#include "G_PlayerController.h"
#include "Components/InputComponent.h"

AG_PlayerController::AG_PlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
}



void AG_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(GContext, 0);
	}
}

void AG_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Triggered, this, &AG_PlayerController::ClickOnGrid);
	}
}

void AG_PlayerController::ClickOnGrid()
{
	const auto HumanPlayer = Cast<AG_HumanPlayer>(GetPawn());
	if (IsValid(HumanPlayer))
	{
		HumanPlayer->OnClick();
	}
}
