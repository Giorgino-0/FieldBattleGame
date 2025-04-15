// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "G_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FIELDBATTLEGAME_API UG_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	int32 FieldSize;
	float ObstaclesPercentage;

public:
	UFUNCTION(BlueprintCallable)
	void SetFieldSizeValueSpinBox(int32 addValue);
	UFUNCTION(BlueprintCallable)
	int32 GetFieldSizeValueSpinBox();
	UFUNCTION(BlueprintCallable)
	void SetObstaclePercentageSpinBox(float addValue);
	UFUNCTION(BlueprintCallable)
	float GetObstaclePercentageSpinBox();

protected:
	TSubclassOf<APawn> SelectedAIClass;

public:
	UFUNCTION(BlueprintCallable)
	void SetSelectedAIClass(TSubclassOf<APawn> addSelectedAIClass);
	UFUNCTION(BlueprintCallable)
	TSubclassOf<APawn> GetSelectedAIClass();

protected:
	FString CurrentAIText;

public:
	UFUNCTION(BlueprintCallable)
	FString GetCurrentAIText();
	UFUNCTION(BlueprintCallable)
	void ChangeAITextToRandom();
	UFUNCTION(BlueprintCallable)
	void ChangeAITextToSmart();

protected:
	FString CurrentTurnText;

public:
	void SetCurrentTurnText(FString addCurrentTurnText);
	UFUNCTION(BlueprintCallable)
	FString GetCurrentTurnText();

protected:
	FString PhaseText;

public:
	void SetPhaseText(FString addPhaseText);
	UFUNCTION(BlueprintCallable)
	FString GetPhaseText();

protected:
	FString CurrentWinnerText;

public:
	void SetCurrentWinnerText(FString addCurrentWinnerText);
	UFUNCTION(BlueprintCallable)
	FString GetCurrentWinnerText();
};
