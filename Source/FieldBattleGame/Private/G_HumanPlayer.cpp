// Fill out your copyright notice in the Description page of Project Settings.


#include "G_HumanPlayer.h"
#include "Sniper_Class.h"
#include "Brawler_Class.h"
#include "G_GameMode.h"

// Sets default values
AG_HumanPlayer::AG_HumanPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	// create a camera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	//set the camera as RootComponent
	SetRootComponent(Camera);

	SpawnList = { { BrawlerBlueprintClass, 1 }, { SniperBlueprintClass, 1 } };

	
}

// Called when the game starts or when spawned
void AG_HumanPlayer::BeginPlay()
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
void AG_HumanPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AG_HumanPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

/*---------------------------------------------------------------------------*/

void AG_HumanPlayer::OnClick()
{
	//Structure containing information about one hit of a trace, such as point of impact and surface normal at that point
	FHitResult Hit = FHitResult(ForceInit);
	// GetHitResultUnderCursor function sends a ray from the mouse position and gives the corresponding hit results
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);
	if (Hit.bBlockingHit) 
	{
		UnitWidgetManagementFunction(Hit);
	}
	if (Hit.bBlockingHit && IsPlayTurn && !bIsDoingSomething)
	{
		ExecutePlayTurn(Hit);
	}
	else if (Hit.bBlockingHit && IsPlacementTurn)
	{
		ExecutePlacementTurn(Hit);
	}
}

/*-------------------- On Placement Click --------------------*/

void AG_HumanPlayer::PlacementTurn()
{	
	IsPlacementTurn = true;
	
	GameInstance->SetCurrentTurnText("HumanPlayerTurn");
}

void AG_HumanPlayer::ExecutePlacementTurn(FHitResult Hit)
{
	if (ToSpawn > 0 && SelectedToSpawn)
	{
		if (ATile* SpawnTile = Cast<ATile>(Hit.GetActor()))
		{
			if (SpawnTile->GetTileStatus() == ETileStatus::EMPTY)
			{
				AMinifigure* SpawnedMinifigure = GameMode->SpawnMinifigure(SelectedClassToSpawn, SpawnTile, GetPlayerColor());
				if (SpawnedMinifigure)
				{
					StartListeningToMinifigure(SpawnedMinifigure);
					OnFieldUnitList.Add(SpawnedMinifigure);
					ToSpawn--;
					SelectedToSpawn = false;
					if (ToSpawn == 0)
					{
						GameMode->PlayControl++;
					}
					for (int32 i = 0; i < SpawnList.Num(); ++i)
					{
						FMinifigureSpawnInfo& Info = SpawnList[i];

						if (Info.MinifigureClass == SelectedClassToSpawn)
						{
							Info.Quantity--;

							if (Info.Quantity <= 0)
							{
								SpawnList.RemoveAt(i);
							}
							break;
						}
					}
					IsPlacementTurn = false;
					GameMode->ControlToStartPlaying();
				}
			}
		}
	}
}

/*-------------------- To Bind the minifigure spawned and to remove it from the list of live unit --------------------*/

void AG_HumanPlayer::RemoveMinifigureFromList(AMinifigure* MinifigureToRemove)
{
	if (MinifigureToRemove) {
		OnFieldUnitList.Remove(MinifigureToRemove);
	}
}

void AG_HumanPlayer::StartListeningToMinifigure(AMinifigure* Minifigure)
{
	Minifigure->OnMinifigureDestroyed.BindUObject(this, &IG_PlayerInterface::RemoveMinifigureFromList);
}

/*-------------------- WidgetFunction --------------------*/

void AG_HumanPlayer::UnitWidgetManagementFunction(FHitResult Hit)
{
	if (AMinifigure* Minifigure = Cast<AMinifigure>(Hit.GetActor()))
	{
		if (bWidgetPresent && MinifigureWidget == Minifigure)
		{
			MinifigureWidget = nullptr;
			bWidgetPresent = false;
			MinifigureDetails->RemoveFromParent();
		}
		else
		{
			SetMinifigureWidget(Minifigure);
		}

	}
}

void AG_HumanPlayer::SetMinifigureWidget(AMinifigure* Minifigure)
{
	if (MinifigureDetails != nullptr) 
	{
		MinifigureDetails->RemoveFromParent();
	}
	MinifigureDetails = CreateWidget<UUserWidget>(GetWorld(), MinifigureDetailsWidget);
	if (MinifigureDetails)
	{
		FObjectProperty* Prop = FindFProperty<FObjectProperty>(MinifigureDetails->GetClass(), FName("MinifigureReference"));

		if (Prop && Prop->PropertyClass->IsChildOf(AMinifigure::StaticClass()))
		{
			Prop->SetObjectPropertyValue_InContainer(MinifigureDetails, Minifigure);
		}
		MinifigureDetails->AddToViewport();
		bWidgetPresent = true;
		MinifigureWidget = Minifigure;
	}
}

/*-------------------- On Play Click --------------------*/

void AG_HumanPlayer::OnPlayTurn()
{
	GameInstance->SetCurrentTurnText("HumanPlayerTurn");
	IsPlayTurn = true;
	bIsDoingSomething = false;
	if (ToPlay == 0)
	{
		IsPlayTurn = false;
		bSelectedMinifigure = false;
		bAttackAfterMovementControl = false;
		bMovementControl = false;
		bAttackControl = false;
		AttackTargets.Empty();
		MovementTargets.Empty();
		for (auto& element : OnFieldUnitList)
		{
			element->bCanPlay = true;
		}
		GameMode->ChangeTurn();
	}
}

void AG_HumanPlayer::ExecutePlayTurn(FHitResult Hit)
{
	//click on a Minifigure
	if (AMinifigure* Minifigure = Cast<AMinifigure>(Hit.GetActor()))
	{
		//click on a my minifigure
		if (OnFieldUnitList.Find(Minifigure) != INDEX_NONE)
		{
			//if the minifigure can play and I don't have any minifigure selected
			if (Minifigure->bCanPlay == true && !bSelectedMinifigure)
			{
				SetMinifigureWidget(Minifigure);
				bSelectedMinifigure = true;
				AttackTargets.Empty();
				MovementTargets.Empty();
				AttackTargets = Minifigure->GetAttackTarget();
				MovementTargets = Minifigure->GetMovementTarget();
				SelectedMinifigure = Minifigure;
				for (const TPair<ATile*, ATile*>& Elem : MovementTargets)
				{
					Elem.Key->HighlightGreen();
				}
				for (AMinifigure* Target : AttackTargets) {
					Target->HighlightRed();
				}
				if (!AttackTargets.IsEmpty())
				{
					bAttackControl = true;
				}
				if (!MovementTargets.IsEmpty())
				{
					bMovementControl = true;
				}
			}
			//if the minifigure can play and I have already a minifigure selected
			else if (Minifigure->bCanPlay == true && bSelectedMinifigure)
			{
				//clic su una mia minifigura per deselezionarla --> deve essere la stessa
				if (Minifigure == SelectedMinifigure)
				{
					if (bAttackAfterMovementControl)
					{
						//if I click on the same minifigure, I lose the possibility to attack after movement
						bAttackAfterMovementControl = false;
						Minifigure->bCanPlay = false;
						bSelectedMinifigure = false;
						for (AMinifigure* Target : AttackTargets) {
							Target->Unhighlight();
						}
						ToPlay--;
						OnPlayTurn();
					}
					else
					{
						bSelectedMinifigure = false;
						bAttackControl = false;
						bMovementControl = true;
						for (const TPair<ATile*, ATile*>& Elem : MovementTargets)
						{
							Elem.Key->Unhighlight();
						}
						for (AMinifigure* Target : AttackTargets) {
							Target->Unhighlight();
						}
					}
				}
			}
		}
		//click on an enemy minifigure
		else if(bAttackControl || bAttackAfterMovementControl)
		{
			if (bSelectedMinifigure)
			{
				bool bControlValue = false;
				for (AMinifigure* PossibleTarget : AttackTargets)
				{
					if (Minifigure == PossibleTarget)
					{
						bControlValue = true;
						break;
					}
				}
				if (bControlValue)
				{
					bAttackAfterMovementControl = false;
					bAttackControl = false;
					bMovementControl = false;
					bSelectedMinifigure = false;
					ToPlay--;
					SelectedMinifigure->bCanPlay = false;
					for (const TPair<ATile*, ATile*>& Elem : MovementTargets)
					{
						Elem.Key->Unhighlight();
					}
					for (AMinifigure* Target : AttackTargets) {
						Target->Unhighlight();
					}
					AttackTargets.Empty();
					MovementTargets.Empty();
					bIsDoingSomething = true;
					SelectedMinifigure->Attack(Minifigure);
				}
			}

		}
		
	}
	//click on a Tile
	else if (ATile* TargetMovementTile = Cast<ATile>(Hit.GetActor())) {
		if (bSelectedMinifigure && bMovementControl)
		{
			bool bControlValue = false;
			for (const TPair<ATile*,ATile*>& Pair : MovementTargets)
			{
				if (TargetMovementTile == Pair.Key)
				{
					bControlValue = true;
					break;
				}
			}
			if (bControlValue)
			{
				bAttackControl = false;
				bMovementControl = false;
				for (const TPair<ATile*, ATile*>& Elem : MovementTargets)
				{
					Elem.Key->Unhighlight();
				}
				bIsDoingSomething = true;
				SelectedMinifigure->MoveTo(MovementTargets, TargetMovementTile);
			}
		}
	}
}

void AG_HumanPlayer::AttackAfterMovement(AMinifigure* Minifigure)
{
	bIsDoingSomething = false;
	AttackTargets.Empty();
	AttackTargets = Minifigure->GetAttackTarget();
	if (!AttackTargets.IsEmpty())
	{
		bAttackAfterMovementControl = true;
		for (AMinifigure* Target : AttackTargets) {
			Target->HighlightRed();
		}
	}
	else
	{
		Minifigure->bCanPlay = false;
		bSelectedMinifigure = false;
		ToPlay--;
		OnPlayTurn();
	}
}

/*-------------------- End of match functions--------------------*/

void AG_HumanPlayer::OnEnd()
{
	IsPlayTurn = false;
	if (!OnFieldUnitList.IsEmpty())
	{
		OnWin();
	}
	else
	{
		OnLose();
	}
}

void AG_HumanPlayer::OnWin()
{
	UE_LOG(LogTemp, Error, TEXT("Win Player"));
	GameInstance->SetCurrentWinnerText("Human Player Win");
}

//useless but could be used 
void AG_HumanPlayer::OnLose()
{
	UE_LOG(LogTemp, Error, TEXT("LosePlayer"));
}

//useless but could be used 
void AG_HumanPlayer::OnDraw()
{
	IsPlayTurn = false;
	UE_LOG(LogTemp, Error, TEXT("Draw"));
}

void AG_HumanPlayer::DestroyPlayer()
{
	for (AMinifigure* Unit : OnFieldUnitList)
	{
		Unit->Destroy();
	}
	this->Destroy();
}

//---------------------------------------------------- Function to call from blueprint

void AG_HumanPlayer::SelectBrawlerToSpawn()
{
	TSubclassOf<AMinifigure> TargetClass = BrawlerBlueprintClass;
	for (int32 i = 0; i < SpawnList.Num(); ++i)
	{
		FMinifigureSpawnInfo& Info = SpawnList[i];

		if (Info.MinifigureClass == TargetClass)
		{
			SelectedToSpawn = true;
			SelectedClassToSpawn = TargetClass;
			break;
		}
	}
}

void AG_HumanPlayer::SelectSniperToSpawn()
{
	TSubclassOf<AMinifigure> TargetClass = SniperBlueprintClass;
	for (int32 i = 0; i < SpawnList.Num(); ++i)
	{
		FMinifigureSpawnInfo& Info = SpawnList[i];

		if (Info.MinifigureClass == TargetClass)
		{
			SelectedToSpawn = true;
			SelectedClassToSpawn = TargetClass;
			break;
		}
	}
}

