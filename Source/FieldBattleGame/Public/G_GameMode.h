// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameField.h"
#include "G_PlayerInterface.h"
#include "G_PlayerController.h"
#include "G_HumanPlayer.h"
#include "G_RandomPlayer.h"
//#include "UObject/UnrealType.h"
//#include "Misc/OutputDeviceNull.h"
//#include "Blueprint/UserWidget.h"
#include "G_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class FIELDBATTLEGAME_API AG_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AG_GameMode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


protected:

	// array of player interfaces
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<IG_PlayerInterface*> PlayersArray;
	//current player 
	int32 CurrentPlayer;
	//first player 
	int32 FirstPlayer;

	// reference to a GameField object
	UPROPERTY(VisibleAnywhere)
	AGameField* GField;
	// TSubclassOf is a template class that provides UClass type safety.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGameField> GameFieldClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AG_RandomPlayer> RandomPlayer;

	//size of the field generated
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 FieldSize;

	//Percentage of obstacles
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ObstaclesPercentage;

	UG_GameInstance* GameInstance;

//protected:
//	UPROPERTY(EditDefaultsOnly)
//	TSubclassOf<AG_IntelligentIA> IntelligentPlayer;
public:
	//control for start playing
	int32 PlayControl;

protected:

	//function to create the map, set the camera and create the players
	void FieldGeneration();
	//function to extract the first player and start the placement phase
	void PlayerGenerationAndCamera();
	//function to decide the first player and then start the placement phase
	void ChoosePlayerAndStartPlacing();

	void StartPlacementPhase();

	void StartPlayPhase();

	int32 TurnNextPlayer(int32 PlayerIndex);

	UFUNCTION(BlueprintCallable, Category = "GameMode")
	void StartGame();
	
	public:
	void LogSpawnedMinifigure(AMinifigure* Minifigure);
	void LogMoveToTile(AMinifigure* Minifigure, ATile* TargetMovementTile);
	void LogAttackMinifigure(AMinifigure* Attacker, AMinifigure* Target, int32 Damage, bool bCounter);
	void LogMinifigureDeath(AMinifigure* Minifigure);
protected:
	FString ChessNotationIfPossible(FVector2D GridPosition);

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> LogsWidgetClass;
	UUserWidget* LogsWidget;*/


public:
	void ControlToStartPlaying();
	AMinifigure* SpawnMinifigure(TSubclassOf<AMinifigure> SpawnClass, ATile* SpawnTile, ETeamColor SpawnColor);
	void ChangeTurn();
	void NowAttack(AMinifigure* Minifigure);
	void ContinueTurn();
	void ControlEnd();
	AGameField* GetGameField();
	int32 GetFieldSize();
	
};
