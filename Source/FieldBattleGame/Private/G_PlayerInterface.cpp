// Fill out your copyright notice in the Description page of Project Settings.


#include "G_PlayerInterface.h"

// Add default functionality here for any IG_PlayerInterface functions that are not pure virtual.

void IG_PlayerInterface::SetPlayerNumber(const int32 addPlayerNumber)
{
	PlayerNumber = addPlayerNumber;
}

void IG_PlayerInterface::SetPlayerColor(const ETeamColor addPlayerColor)
{
	PlayerColor = addPlayerColor;
}

const int32 IG_PlayerInterface::GetPlayerNumber()
{
	return PlayerNumber;
}

const ETeamColor IG_PlayerInterface::GetPlayerColor()
{
	return PlayerColor;
}

TArray<AMinifigure*>& IG_PlayerInterface::GetOnFieldUnitList()
{
	return OnFieldUnitList;
}
