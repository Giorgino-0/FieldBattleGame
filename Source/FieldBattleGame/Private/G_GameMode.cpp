// Fill out your copyright notice in the Description page of Project Settings.


#include "G_GameMode.h"


AG_GameMode::AG_GameMode()
{
	PlayerControllerClass = AG_PlayerController::StaticClass();
	DefaultPawnClass = AG_HumanPlayer::StaticClass();
	FieldSize = 25;
	ObstaclesPercentage = 0.3f;
}

AGameField* AG_GameMode::GetGameField()
{
	return GField;
}

void AG_GameMode::BeginPlay()
{
	Super::BeginPlay();
	GameInstance = Cast<UG_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("No Gameinstance founded"));
		return;
	}
	/*LogsWidget = CreateWidget<UUserWidget>(GetWorld(), LogsWidgetClass);
	if (!LogsWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("No LogsWidget founded"));
		return;
	}*/
	GameInstance->FieldSize = FieldSize;
	GameInstance->ObstaclesPercentage = ObstaclesPercentage;
	GameInstance->SetSelectedAIClass(RandomPlayer);
	GameInstance->ChangeAITextToRandom();
}

/*---------------------------------------------------------------------------*/

void AG_GameMode::StartGame()
{
	FieldGeneration();
	PlayerGenerationAndCamera();
	ChoosePlayerAndStartPlacing();
}

void AG_GameMode::FieldGeneration()
{
	FieldSize = GameInstance->FieldSize;
	ObstaclesPercentage = GameInstance->ObstaclesPercentage;
	if (GameFieldClass != nullptr)
	{
		GField = GetWorld()->SpawnActor<AGameField>(GameFieldClass);
		GField->SetFieldSize(FieldSize);
		GField->SetObstaclesPercentage(ObstaclesPercentage);
		GField->CreateGameField();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Game Field is null"));
	}
}

void AG_GameMode::PlayerGenerationAndCamera()
{
	AG_HumanPlayer* HumanPlayer = GetWorld()->GetFirstPlayerController()->GetPawn<AG_HumanPlayer>();
	if (!IsValid(HumanPlayer))
	{
		UE_LOG(LogTemp, Error, TEXT("No player pawn of type '%s' was found."), *AG_HumanPlayer::StaticClass()->GetName());
		return;
	}

	float CameraPosX = (GField->GetTileDimension() + (GField->GetTileDimension() * GField->GetTilePadding())) * (FieldSize - 1) * 0.5f;
	float Zposition = FieldSize * 140;
	FVector CameraPos(CameraPosX, CameraPosX, Zposition);
	HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());

	HumanPlayer->SetPlayerColor(ETeamColor::BLUE);
	PlayersArray.Add(HumanPlayer);

	if (RandomPlayer == GameInstance->GetSelectedAIClass())
	{
		auto* AI = GetWorld()->SpawnActor<AG_RandomPlayer>(RandomPlayer);
		if (!IsValid(AI))
		{
			UE_LOG(LogTemp, Error, TEXT("No Artificial Intelligence adversary founded"));
			return;
		}
		AI->SetPlayerColor(ETeamColor::RED);
		PlayersArray.Add(AI);
	}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("SmartArtificialIntelligence"));
	//	/**/* AI = GetWorld()->SpawnActor</**/ >( /**/);
	//	if (!IsValid(AI))
	//	{
	//		UE_LOG(LogTemp, Error, TEXT("No Artificial Intelligence adversary founded"));
	//		return;
	//	}
	//	AI->SetPlayerColor(ETeamColor::RED);
	//	PlayersArray.Add(AI);
	//}
}

void AG_GameMode::ChoosePlayerAndStartPlacing()
{
	FirstPlayer = FMath::RandRange(0, PlayersArray.Num() - 1);
	CurrentPlayer = FirstPlayer;
	for (int32 IndexI = 0; IndexI < PlayersArray.Num(); IndexI++)
	{
		PlayersArray[IndexI]->SetPlayerNumber(IndexI);
	}
	PlayControl = 0;
	GameInstance->SetPhaseText("Placement Phase");
	FTimerHandle WidgetTimer;
	GetWorld()->GetTimerManager().SetTimer(WidgetTimer, this, &AG_GameMode::StartPlacementPhase, 3.0f, false);
}

/*---------------------------------------------------------------------------*/

void AG_GameMode::StartPlacementPhase()
{
	GameInstance->SetPhaseText("end");
	PlayersArray[CurrentPlayer]->PlacementTurn();
}

void AG_GameMode::ControlToStartPlaying()
{
	if (PlayControl != PlayersArray.Num())
	{
		CurrentPlayer = TurnNextPlayer(CurrentPlayer);
		PlayersArray[CurrentPlayer]->PlacementTurn();
	}
	else
	{
		CurrentPlayer = FirstPlayer;
		PlayersArray[CurrentPlayer]->ToPlay = PlayersArray[CurrentPlayer]->GetOnFieldUnitList().Num();
		GameInstance->SetPhaseText("Fight");
		FTimerHandle WidgetTimer;
		GetWorld()->GetTimerManager().SetTimer(WidgetTimer, this, &AG_GameMode::StartPlayPhase, 3.0f, false);
		
	}
}

void AG_GameMode::StartPlayPhase()
{
	GameInstance->SetPhaseText("end");
	PlayersArray[CurrentPlayer]->OnPlayTurn();
}

int32 AG_GameMode::TurnNextPlayer(int32 PlayerIndex)
{
	PlayerIndex++;
	if (!PlayersArray.IsValidIndex(PlayerIndex))
	{
		PlayerIndex = 0;
	}
	return PlayerIndex;
}

AMinifigure* AG_GameMode::SpawnMinifigure(TSubclassOf<AMinifigure> SpawnClass, ATile* SpawnTile, ETeamColor SpawnColor)
{
	FVector SpawnTileLocation = SpawnTile->GetActorLocation();
	FVector Location(SpawnTileLocation.X, SpawnTileLocation.Y, (SpawnTileLocation.Z + (SpawnTile->GetStaticMeshScale().Z * 100.f)));
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(Location);
	SpawnTransform.SetRotation(FQuat(FRotator::ZeroRotator));
	SpawnTransform.SetScale3D(FVector(1.0f));
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.bDeferConstruction = true;
	AMinifigure* SpawnedMinifigure = GetWorld()->SpawnActor<AMinifigure>(SpawnClass, SpawnTransform, SpawnParams);
	SpawnTile->SetOccupant(SpawnedMinifigure);
	if (SpawnedMinifigure)
	{
		SpawnedMinifigure->SetTeamColor(SpawnColor);
		SpawnedMinifigure->SetMaterialByTeamColor();
		SpawnedMinifigure->SetActorScale3D(FVector(SpawnTile->GetActorScale3D().X, SpawnTile->GetActorScale3D().Y, 1));
		SpawnedMinifigure->bCanPlay = true;
		SpawnedMinifigure->SetCurrentPosition(SpawnTile);
		SpawnTile->SetTileStatus(ETileStatus::OCCUPIED);
		if (PlayersArray[CurrentPlayer]->GetPlayerColor() == ETeamColor::RED)
		{
			SpawnTile->SetTileOwner(ETileOwner::RED);
		}
		else
		{
			SpawnTile->SetTileOwner(ETileOwner::BLUE);
		}
		SpawnedMinifigure->FinishSpawning(SpawnTransform);
		LogSpawnedMinifigure(SpawnedMinifigure);
	}
	return SpawnedMinifigure;
}

int32 AG_GameMode::GetFieldSize()
{
	return FieldSize;
}

void AG_GameMode::ChangeTurn()
{
	CurrentPlayer = TurnNextPlayer(CurrentPlayer);
	PlayersArray[CurrentPlayer]->ToPlay = PlayersArray[CurrentPlayer]->GetOnFieldUnitList().Num();
	PlayersArray[CurrentPlayer]->OnPlayTurn();
}

void AG_GameMode::NowAttack(AMinifigure* Minifigure)
{
	PlayersArray[CurrentPlayer]->AttackAfterMovement(Minifigure);
}

void AG_GameMode::ContinueTurn()
{
	PlayersArray[CurrentPlayer]->OnPlayTurn();
}

void AG_GameMode::ControlEnd()
{
	int32 counter = PlayersArray.Num();

	for (int index=0; index < PlayersArray.Num(); index++)
	{
		if (PlayersArray[index]->GetOnFieldUnitList().IsEmpty())
		{
			counter--;
		}
	}
	if (counter > 1)
	{
		ContinueTurn();
	}
	else
	{
		GameInstance->SetCurrentTurnText("End of match");
		if (counter == 0)
		{
			GameInstance->SetCurrentWinnerText("It's a draw");
			for (int index = 0; index < PlayersArray.Num(); index++)
			{
				PlayersArray[index]->OnDraw();
			}
		}
		else
		{
			for (int index = 0; index < PlayersArray.Num(); index++)
			{
				PlayersArray[index]->OnEnd();
			}
		}
	}
}

/*---------------------------------------------------------------------------*/

void AG_GameMode::LogSpawnedMinifigure(AMinifigure* Minifigure)
{
	FString GridFormatted = ChessNotationIfPossible(Minifigure->GetCurrentPosition()->GetTileGridPosition());
	
	FString Team = StaticEnum<ETeamColor>()->GetDisplayNameTextByValue(static_cast<int64>(Minifigure->GetTeamColor())).ToString();
	FString Type = StaticEnum<EMinifigureType>()->GetDisplayNameTextByValue(static_cast<int64>(Minifigure->GetMinifigureType())).ToString();
	FString Message = FString::Printf(TEXT("Team %s: %s spawn in %s"), *Team, *Type, *GridFormatted);
	/*FString Command = FString::Printf(TEXT("AddMessage \"%s\""), *Message);
	FOutputDeviceNull Ar;
	LogsWidget->CallFunctionByNameWithArguments(*Command, Ar, nullptr, true);*/
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, Message);
	
	
}

void AG_GameMode::LogMoveToTile(AMinifigure* Minifigure, ATile* TargetMovementTile)
{
	FString StartGridFormatted = ChessNotationIfPossible(Minifigure->GetCurrentPosition()->GetTileGridPosition());
	FString ArriveGridFormatted = ChessNotationIfPossible(TargetMovementTile->GetTileGridPosition());
	
	FString Team = StaticEnum<ETeamColor>()->GetDisplayNameTextByValue(static_cast<int64>(Minifigure->GetTeamColor())).ToString();
	FString Type = StaticEnum<EMinifigureType>()->GetDisplayNameTextByValue(static_cast<int64>(Minifigure->GetMinifigureType())).ToString();
	FString Message = FString::Printf(TEXT("Team %s: %s from %s to %s"), *Team, *Type, *StartGridFormatted, *ArriveGridFormatted);
	/*FString Command = FString::Printf(TEXT("AddMessage \"%s\""), *Message);
	FOutputDeviceNull Ar;
	LogsWidget->CallFunctionByNameWithArguments(*Command, Ar, nullptr, true);*/
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, Message);
}



void AG_GameMode::LogAttackMinifigure(AMinifigure* Attacker, AMinifigure* Target, int32 Damage, bool bCounter)
{
	FString AttackerTeam = StaticEnum<ETeamColor>()->GetDisplayNameTextByValue(static_cast<int64>(PlayersArray[CurrentPlayer]->GetPlayerColor())).ToString();
	FString AttackerType = StaticEnum<EMinifigureType>()->GetDisplayNameTextByValue(static_cast<int64>(Attacker->GetMinifigureType())).ToString();
	
	FString TargetTeam = StaticEnum<ETeamColor>()->GetDisplayNameTextByValue(static_cast<int64>(PlayersArray[CurrentPlayer]->GetPlayerColor())).ToString();
	FString TargetType = StaticEnum<EMinifigureType>()->GetDisplayNameTextByValue(static_cast<int64>(Attacker->GetMinifigureType())).ToString();
	FString TargetPosition = ChessNotationIfPossible(Target->GetCurrentPosition()->GetTileGridPosition());

	FString DamageValue = FString::FromInt(Damage);

	FString Message;
	if (!bCounter)
	{
		Message = FString::Printf(TEXT("Team %s: %s attack to %s:%s at %s, for %s damage"), *AttackerTeam, *AttackerType, *TargetTeam, *TargetType, *TargetPosition, *DamageValue);
	}
	else
	{
		Message = FString::Printf(TEXT("Team %s: %s counter attack to %s:%s at %s, for %s damage"), *AttackerTeam, *AttackerType, *TargetTeam, *TargetType, *TargetPosition, *DamageValue);
	}
	/*FString Command = FString::Printf(TEXT("AddMessage \"%s\""), *Message);
	FOutputDeviceNull Ar;
	LogsWidget->CallFunctionByNameWithArguments(*Command, Ar, nullptr, true);*/
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, Message);
	
}
void AG_GameMode::LogMinifigureDeath(AMinifigure* Minifigure)
{
	FString Team = StaticEnum<ETeamColor>()->GetDisplayNameTextByValue(static_cast<int64>(Minifigure->GetTeamColor())).ToString();
	FString Type = StaticEnum<EMinifigureType>()->GetDisplayNameTextByValue(static_cast<int64>(Minifigure->GetMinifigureType())).ToString();
	FString Message = FString::Printf(TEXT("Death of %s:%s "), *Team, *Type);
	/*FString Command = FString::Printf(TEXT("AddMessage \"%s\""), *Message);
	FOutputDeviceNull Ar;
	LogsWidget->CallFunctionByNameWithArguments(*Command, Ar, nullptr, true);*/
	FTimerHandle WidgetTimer;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, Message);
}

FString AG_GameMode::ChessNotationIfPossible(FVector2D GridPosition)
{
	
	if ( FieldSize <= 25)
	{
		int32 X = FMath::RoundToInt(GridPosition.X);
		int32 Y = FMath::RoundToInt(GridPosition.Y);
		TCHAR Col = 'A' + X;
		int32 Row = Y + 1;
		return FString::Printf(TEXT("%c%d"), Col, Row);
	}
	else
	{
		return FString::Printf(TEXT("(%d,%d)"), static_cast<int32>(GridPosition.X), static_cast<int32>(GridPosition.Y));
	}
}