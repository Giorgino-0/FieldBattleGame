// Fill out your copyright notice in the Description page of Project Settings.


#include "G_RandomPlayer.h"
#include "Sniper_Class.h"
#include "Brawler_Class.h"
#include "G_GameMode.h"


// Sets default values
AG_RandomPlayer::AG_RandomPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpawnList = { { ABrawler_Class::StaticClass(), 1 }, { ASniper_Class::StaticClass(), 1 } };

}

// Called when the game starts or when spawned
void AG_RandomPlayer::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<AG_GameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("No Gamemode founded"));
		return;
	}
	GameInstance = Cast<UG_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("No Gamemode founded"));
		return;
	}
	ToSpawn = 0;
	for (auto element : SpawnList) {
		ToSpawn = ToSpawn + element.Quantity;
	}
	ToPlay = 0;
}

// Called every frame
void AG_RandomPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AG_RandomPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

/*---------------------------------------------------------------------------*/

/*-------------------- On Placement Phase --------------------*/

void AG_RandomPlayer::PlacementTurn()
{
	GameInstance->SetCurrentTurnText("RandomIATurn");
	FTimerHandle AIThinkingTimer;
	GetWorld()->GetTimerManager().SetTimer(AIThinkingTimer, this, &AG_RandomPlayer::ExecutePlacementTurn, 3.0f, false);
}

void AG_RandomPlayer::ExecutePlacementTurn()
{
	AGameField* Field = GameMode->GetGameField();
	//controllo sul numero di pedine schierabili
	if (ToSpawn > 0) {
		//estraggo una posizione random 
		ATile* SpawnTile = GetRandomFreeTile(Field);
		if (SpawnTile == nullptr)
		{
			//TODO
			return;
		}
		//estraggo la classe da spawnare
		TSubclassOf<AMinifigure> SpawnClass = GetRandomSpawnClass();
		if (SpawnClass == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("No Possible Spawn founded founded"));
			return;
		}
		//spawn vero e proprio
		AMinifigure* SpawnedMinifigure = GameMode->SpawnMinifigure(SpawnClass, SpawnTile, GetPlayerColor());
		if (SpawnedMinifigure)
		{
			StartListeningToMinifigure(SpawnedMinifigure);
			OnFieldUnitList.Add(SpawnedMinifigure);
			ToPlay++;
			ToSpawn--;
			if (ToSpawn == 0)
			{
				GameMode->PlayControl++;
			}
			//LogSpawnedMinifigure(SpawnedMinifigure);
			//se voglio posso introdure un timer per fare in modo che finisca la funzione e non cresca lo stack
			FTimerHandle Timer;
			GetWorld()->GetTimerManager().SetTimer(Timer, GameMode, &AG_GameMode::ControlToStartPlaying, 0.2f, false);
		}
	}
	else
	{
		GameMode->ControlToStartPlaying();
	}
}

TSubclassOf<AMinifigure> AG_RandomPlayer::GetRandomSpawnClass()
{
	if (SpawnList.Num() == 0)
	{
		return nullptr;
	}
	for (int i = 0; i < SpawnList.Num(); i++)
	{
		if (SpawnList[i].Quantity == 0)
		{
			SpawnList.RemoveAt(i);
		}
	}
	while (1) {
		int32 Index = FMath::RandRange(0, SpawnList.Num() - 1);
		TSubclassOf<AMinifigure> SpawnClass = SpawnList[Index].MinifigureClass;
		SpawnList[Index].Quantity--;
		if (SpawnList[Index].Quantity == 0)
		{
			SpawnList.RemoveAt(Index);
		}
		return SpawnClass;
	}
}

/*-------------------- To Bind the minifigure spawned and to remove it from the list of live unit --------------------*/

void AG_RandomPlayer::RemoveMinifigureFromList(AMinifigure* MinifigureToRemove)
{
	if (MinifigureToRemove) {
		OnFieldUnitList.Remove(MinifigureToRemove);
	}

}

void AG_RandomPlayer::StartListeningToMinifigure(AMinifigure* Minifigure)
{
	Minifigure->OnMinifigureDestroyed.BindUObject(this, &AG_RandomPlayer::RemoveMinifigureFromList);
}

/*-------------------- On Play Phase --------------------*/

void AG_RandomPlayer::OnPlayTurn()
{
	GameInstance->SetCurrentTurnText("RandomIATurn");
	if (ToPlay > 0)
	{
		ToPlay--;
		int32 Index = GetRandomPlayableUnit();
		if (Index == -1)
		{
			UE_LOG(LogTemp, Error, TEXT("Non minifigure schierate"));
			return;
		}
		AMinifigure* CurrentMinifigure = OnFieldUnitList[Index];
		CurrentMinifigure->bCanPlay = false;

		TArray<AMinifigure* > AttackTargets = CurrentMinifigure->GetAttackTarget();
		TMap<ATile*, ATile*> MovementTargets = CurrentMinifigure->GetMovementTarget();
		for (const TPair<ATile*, ATile*>& Elem : MovementTargets)
		{
			Elem.Key->HighlightGreen();
		}
		for (AMinifigure* Target : AttackTargets) {
			Target->HighlightRed();
		}

		FTimerHandle AIThinkingTimer;
		GetWorld()->GetTimerManager().SetTimer(AIThinkingTimer, [this, MovementTargets, AttackTargets, CurrentMinifigure, Index]()
			{
				if (!MovementTargets.IsEmpty() && !AttackTargets.IsEmpty())
				{
					if (FMath::RandBool())
					{
						for (const TPair<ATile*, ATile*>& Elem : MovementTargets)
						{
							Elem.Key->Unhighlight();
						}
						RandomAttack(AttackTargets, CurrentMinifigure);
					}
					else
					{	
						for (AMinifigure* Target : AttackTargets) {
							Target->Unhighlight();
						}
						RandomMovement(MovementTargets, CurrentMinifigure);
					}
				}
				else if (!MovementTargets.IsEmpty())
				{
					for (AMinifigure* Target : AttackTargets) {
						Target->Unhighlight();
					}
					RandomMovement(MovementTargets, CurrentMinifigure);
				}
				else if (!AttackTargets.IsEmpty())
				{
					for (const TPair<ATile*, ATile*>& Elem : MovementTargets)
					{
						Elem.Key->Unhighlight();
					}
					RandomAttack(AttackTargets, CurrentMinifigure);
				}
			}, 3.0f, false);
	}
	else
	{
		for (auto& element : OnFieldUnitList)
		{
			element->bCanPlay = true;
		}
		GameMode->ChangeTurn();
	}
	
}

int32 AG_RandomPlayer::GetRandomPlayableUnit()
{
	if (OnFieldUnitList.IsEmpty()) return -1;

	int32 Index = FMath::RandRange(0, OnFieldUnitList.Num() - 1);

	int32 Attempts = 0;
	const int32 MaxAttempts = 100*OnFieldUnitList.Num();

	while (!OnFieldUnitList[Index]->bCanPlay && Attempts < MaxAttempts)
	{
		Index = FMath::RandRange(0, OnFieldUnitList.Num() - 1);
		Attempts++;
	}
	if (Attempts < MaxAttempts) return Index;
	else return -1;
}

ATile* AG_RandomPlayer::GetRandomFreeTile(AGameField* Field)
{
	TArray<ATile*>& FreeTiles = Field->GetFreeTilesStartArray();
	if (FreeTiles.Num() > 0)
	{
		int32 control = FreeTiles.Num();
		while (control > 0)
		{
			int32 Index = FMath::RandRange(0, FreeTiles.Num() - 1);
			if (FreeTiles[Index]->GetTileStatus() == ETileStatus::EMPTY)
			{
				return FreeTiles[Index];
			}
			else
			{
				control--;
			}
		}
	}
	return nullptr;
}

void AG_RandomPlayer::RandomMovement(TMap<ATile*, ATile*> MovementTargets, AMinifigure* CurrentMinifigure)
{
	TArray<ATile*> Key;
	MovementTargets.GenerateKeyArray(Key);
	int32 Index = FMath::RandRange(0, Key.Num() - 1);
	if (Key[Index] == CurrentMinifigure->GetCurrentPosition()) {
		UE_LOG(LogTemp, Error, TEXT("Game movement error"));
	}
	FTimerHandle AIThinkingTimer;
	GetWorld()->GetTimerManager().SetTimer(AIThinkingTimer, [this, Key, MovementTargets, CurrentMinifigure, Index]()
		{
			for (const TPair<ATile*, ATile*>& Elem : MovementTargets)
			{
				Elem.Key->Unhighlight();
			}
			CurrentMinifigure->MoveTo(MovementTargets, Key[Index]);
		}, 2.0f, false);

}

void AG_RandomPlayer::RandomAttack(TArray< AMinifigure*> AttackTargets, AMinifigure* CurrentMinifigure)
{
	int32 Index = FMath::RandRange(0, AttackTargets.Num() - 1);
	
	FTimerHandle AIThinkingTimer;
	GetWorld()->GetTimerManager().SetTimer(AIThinkingTimer, [this, AttackTargets, CurrentMinifigure, Index]()
		{
			for (AMinifigure* Target : AttackTargets) {
				Target->Unhighlight();
			}
			CurrentMinifigure->Attack(AttackTargets[Index]);
		}, 2.0f, false);
}

void AG_RandomPlayer::AttackAfterMovement(AMinifigure* Minifigure)
{
	TArray<AMinifigure*> AttackTargets = Minifigure->GetAttackTarget();
	for (AMinifigure* Target : AttackTargets) {
		Target->HighlightRed();
	}
	if (!AttackTargets.IsEmpty() && FMath::RandBool())
	{
		RandomAttack(AttackTargets, Minifigure);
	}
	else
	{
		FTimerHandle AIThinkingTimer;
		GetWorld()->GetTimerManager().SetTimer(AIThinkingTimer, [this, AttackTargets]()
			{
				for (AMinifigure* Target : AttackTargets) {
					Target->Unhighlight();
				}
				GameMode->ContinueTurn();
			}, 2.0f, false);
	}
}

/*-------------------- End of match functions--------------------*/

void AG_RandomPlayer::OnEnd()
{
	if (!OnFieldUnitList.IsEmpty())
	{
		OnWin();
	}
	else
	{
		OnLose();
	}
}

void AG_RandomPlayer::OnWin()
{
	UE_LOG(LogTemp, Error, TEXT("WinAI"));
	GameInstance->SetCurrentWinnerText("AI Player Win");
}

void AG_RandomPlayer::OnLose()
{
	UE_LOG(LogTemp, Error, TEXT("AILose"));
}

void AG_RandomPlayer::OnDraw()
{
	UE_LOG(LogTemp, Error, TEXT("DrawAI"));
}

void AG_RandomPlayer::DestroyPlayer()
{
	for (AMinifigure* Unit : OnFieldUnitList)
	{
		Unit->Destroy();
	}
	this->Destroy();
}