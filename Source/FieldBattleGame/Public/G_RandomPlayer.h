// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "G_PlayerInterface.h"
#include "Tile.h"
#include "GameField.h"
#include "G_GameInstance.h"
#include "G_RandomPlayer.generated.h"

UCLASS()
class FIELDBATTLEGAME_API AG_RandomPlayer : public APawn, public IG_PlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AG_RandomPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	AG_GameMode* GameMode;
	UG_GameInstance* GameInstance;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TArray<FMinifigureSpawnInfo> SpawnList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ToSpawn;

	int32 MaxNumberUnitToPlay;

public:

	virtual void PlacementTurn() override;
	virtual void OnPlayTurn() override;
	virtual void AttackAfterMovement(AMinifigure* Minifigure) override;
	virtual void OnEnd() override;
	virtual void OnDraw() override;
	virtual void DestroyPlayer() override;
	virtual void RemoveMinifigureFromList(AMinifigure* MinifigureToRemove) override;

protected:

	ATile* GetRandomFreeTile(AGameField* Field);
	TSubclassOf<AMinifigure> GetRandomSpawnClass();
	void ExecutePlacementTurn();
	int32 GetRandomPlayableUnit();
	void RandomMovement(TMap<ATile*, ATile*> MovementTarget, AMinifigure* CurrentMinifigure);
	void RandomAttack(TArray< AMinifigure*> AttackTarget, AMinifigure* CurrentMinifigure);
	virtual void StartListeningToMinifigure(AMinifigure* Minifigure) override;
	virtual void OnWin() override;
	virtual void OnLose() override;

};
