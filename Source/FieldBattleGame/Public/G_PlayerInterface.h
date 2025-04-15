// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Minifigure.h"
#include "G_PlayerInterface.generated.h"





USTRUCT(BlueprintType)
struct FMinifigureSpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AMinifigure> MinifigureClass;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 Quantity;
};



// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UG_PlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FIELDBATTLEGAME_API IG_PlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

protected:
	
	int32 PlayerNumber;
	ETeamColor PlayerColor;
	TArray<AMinifigure*> OnFieldUnitList;
public:
	//for an easier use
	int32 ToPlay;

public:
	//access method
	void SetPlayerNumber(const int32 addPlayerNumber);
	const int32 GetPlayerNumber();
	void SetPlayerColor(const ETeamColor addPlayerColor);
	const ETeamColor GetPlayerColor();
	TArray<AMinifigure*>& GetOnFieldUnitList();

public:
	//all of the function that have to be called to the GameMode over the Players
	virtual void PlacementTurn() = 0;
	virtual void OnPlayTurn() = 0;
	virtual void AttackAfterMovement(AMinifigure* Minifigure) = 0;
	virtual void OnEnd() = 0; 
	virtual void OnDraw() = 0;
	virtual void DestroyPlayer() = 0;
	virtual void RemoveMinifigureFromList(AMinifigure* MinifigureToRemove) = 0;

protected:

	virtual void StartListeningToMinifigure(AMinifigure* Minifigure) = 0;
	virtual void OnWin() = 0;
	virtual void OnLose() = 0;
};
