// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "G_HumanPlayer.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "G_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FIELDBATTLEGAME_API AG_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AG_PlayerController();

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* GContext;


	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ClickAction;

	void ClickOnGrid();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;


};
