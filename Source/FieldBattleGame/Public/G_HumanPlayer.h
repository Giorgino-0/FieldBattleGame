// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "G_PlayerInterface.h"
#include "G_GameInstance.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "G_HumanPlayer.generated.h"

UCLASS()
class FIELDBATTLEGAME_API AG_HumanPlayer : public APawn, public IG_PlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AG_HumanPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	//camera component attacched to player pawn
	UCameraComponent* Camera;

protected:

	AG_GameMode* GameMode;
	UG_GameInstance* GameInstance;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TArray<FMinifigureSpawnInfo> SpawnList;

	UPROPERTY(VisibleAnywhere)
	int32 ToSpawn;

	TSubclassOf<AMinifigure> SelectedClassToSpawn;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<AMinifigure> SniperBlueprintClass;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<AMinifigure> BrawlerBlueprintClass;

	TArray<AMinifigure* > AttackTargets;
	TMap<ATile*, ATile*> MovementTargets;
	AMinifigure* SelectedMinifigure;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> MinifigureDetailsWidget;

	UUserWidget* MinifigureDetails;
	AMinifigure* MinifigureWidget;

protected:

	bool IsPlacementTurn = false;
	bool IsPlayTurn = false;
	bool SelectedToSpawn = false;
	bool bSelectedMinifigure = false;
	bool bAttackAfterMovementControl = false;
	bool bAttackControl = false;
	bool bMovementControl = false;
	bool bIsDoingSomething = false;
	bool bWidgetPresent = false;

public:

	virtual void PlacementTurn() override;
	virtual void OnPlayTurn() override;
	virtual void AttackAfterMovement(AMinifigure* Minifigure) override;
	virtual void OnEnd() override;
	virtual void OnDraw() override;
	virtual void DestroyPlayer() override;
	virtual void RemoveMinifigureFromList(AMinifigure* MinifigureToRemove) override;

public:
	// called on left mouse click (binding)
	UFUNCTION()
	void OnClick();

protected:
	void ExecutePlacementTurn(FHitResult Hit);
	void UnitWidgetManagementFunction(FHitResult Hit);
	void SetMinifigureWidget(AMinifigure* Minifigure);
	void ExecutePlayTurn(FHitResult Hit);
	virtual void StartListeningToMinifigure(AMinifigure* Minifigure) override;
	virtual void OnWin() override;
	virtual void OnLose() override;

protected:
	UFUNCTION(BlueprintCallable, Category = "HumanPlayer")
	void SelectBrawlerToSpawn();
	UFUNCTION(BlueprintCallable, Category = "HumanPlayer")
	void SelectSniperToSpawn();
};
