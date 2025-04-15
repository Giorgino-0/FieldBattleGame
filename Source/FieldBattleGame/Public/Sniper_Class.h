// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minifigure.h"
#include "Sniper_Class.generated.h"

/**
 * 
 */
UCLASS()
class FIELDBATTLEGAME_API ASniper_Class : public AMinifigure
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASniper_Class();
	bool Counter(AMinifigure* CounterTarget) override;
};
