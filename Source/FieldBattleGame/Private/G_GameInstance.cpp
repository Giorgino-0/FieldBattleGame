// Fill out your copyright notice in the Description page of Project Settings.


#include "G_GameInstance.h"

void UG_GameInstance::SetFieldSizeValueSpinBox(int32 addValue)
{
	FieldSize = addValue;
}

void UG_GameInstance::SetObstaclePercentageSpinBox(float addValue)
{
	ObstaclesPercentage = addValue;
}

int32 UG_GameInstance::GetFieldSizeValueSpinBox()
{
	return FieldSize;
}

float UG_GameInstance::GetObstaclePercentageSpinBox()
{
	return ObstaclesPercentage;
}

void UG_GameInstance::SetSelectedAIClass(TSubclassOf<APawn> addSelectedAIClass)
{
	SelectedAIClass = addSelectedAIClass;
}

TSubclassOf<APawn> UG_GameInstance::GetSelectedAIClass()
{
	return SelectedAIClass;
}

FString UG_GameInstance::GetCurrentAIText()
{
	return CurrentAIText;
}

void UG_GameInstance::ChangeAITextToRandom()
{
	CurrentAIText = "RandomAI";
}

void UG_GameInstance::ChangeAITextToSmart()
{
	CurrentAIText = "IntelligentAI";
}

void UG_GameInstance::SetCurrentTurnText(FString addCurrentTurnText)
{
	CurrentTurnText = addCurrentTurnText;
}
FString UG_GameInstance::GetCurrentTurnText()
{
	return CurrentTurnText;
}

void UG_GameInstance::SetPhaseText(FString addPhaseText) {
	PhaseText = addPhaseText;
}

FString UG_GameInstance::GetPhaseText() {
	return PhaseText;
}


void UG_GameInstance::SetCurrentWinnerText(FString addCurrentWinnerText)
{
	CurrentWinnerText = addCurrentWinnerText;
}

FString UG_GameInstance::GetCurrentWinnerText()
{
	return CurrentWinnerText;
}



