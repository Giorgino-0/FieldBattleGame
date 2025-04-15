// Fill out your copyright notice in the Description page of Project Settings.


#include "GameField.h"

// Sets default values
AGameField::AGameField()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	FieldSize = 25;
	TileDimension = 120.f;
	TilePadding = 0.f;
	ObstaclesPercentage = 0.4f;

}

void AGameField::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	NextCellPositionMultiplier = (TileDimension + (TileDimension * TilePadding)) / TileDimension;
}

// Called when the game starts or when spawned
void AGameField::BeginPlay()
{
	Super::BeginPlay();
}

/*---------------------------------------------------------------------------*/

void AGameField::CreateGameField()
{
	GenerateEmptyField();
	Fill_NearbyTilesList();
	LabirinthGenerator();
	GenerateObstacle();
}

void AGameField::DestroyField()
{
	for (ATile* Tile : TilesArray)
	{
		Tile->Destroy();
	}
	for (AObstacle* Obstacle : ObstaclesArray)
	{
		Obstacle->Destroy();
	}
	this->Destroy();
}

/*---------------------------------------------------------------------------*/

FVector AGameField::GetPositionFromXY(const int32 X_value, const int32 Y_value)
{
	return TileDimension * NextCellPositionMultiplier * FVector(X_value, Y_value, 0);
}

void AGameField::GenerateEmptyField()
{	
	TilesArray.Empty();
	TilesMap.Empty();
	TileIndexMap.Empty();
	for (int32 index_X = 0; index_X < FieldSize; index_X++)
	{
		for (int32 index_Y = 0; index_Y < FieldSize; index_Y++) {
			FVector Location = GetPositionFromXY(index_X, index_Y);
			ATile* Obj = GetWorld()->SpawnActor<ATile>(TilesClass, Location, FRotator::ZeroRotator);
			const float TileScale = TileDimension / 100.f;
			Obj->SetActorScale3D(FVector(TileScale, TileScale, 1));
			Obj->SetTileGridPosition(index_X, index_Y);
			TilesArray.Add(Obj);
			TilesMap.Add(FVector2D(index_X, index_Y), Obj);
			TileIndexMap.Add(Obj, TilesArray.Num() - 1);
		}
	}
}

void AGameField::Fill_NearbyTilesList()
{
	//vector of possible movement to find nearby tiles
	TArray<FVector2D> possible_directions = { {1,0},{-1,0},{0,1},{0,-1} };
	for (auto& tile : TilesArray)
	{
		tile->GetNearbyTilesList().Empty();
		for (auto& dir : possible_directions)
		{
			int newX = tile->GetTileGridPosition().X + StaticCast<int32>(dir.X);
			int newY = tile->GetTileGridPosition().Y + StaticCast<int32>(dir.Y);
			auto it = TilesMap.FindRef(FVector2D(newX, newY));
			if (it != nullptr) {
				tile->GetNearbyTilesList().Add(it);
			}
		}
	}
}

void AGameField::LabirinthGenerator()
{
	if (TilesArray.IsEmpty())
	{
		return;
	}

	TArray<EDFSColor> Colors;
	Colors.Init(EDFSColor::WHITE, TilesArray.Num());

	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<int> distribution(0, TilesArray.Num() - 1);

	ATile* first_tile = TilesArray[distribution(generator)];

	Labirinth(first_tile, Colors, generator);
}

int AGameField::Labirinth(ATile* current_tile, TArray<EDFSColor>& Colors, std::mt19937& generator)
{
	
	int32* TileIndexPtr = TileIndexMap.Find(current_tile);
	
	if (!TileIndexPtr) return 0;
	
	int32 TileIndex = *TileIndexPtr;
	
	TileProfundityMap.Add(current_tile, 0);
	
	Colors[TileIndex] = EDFSColor::GREY;
	
	if (current_tile->GetNearbyTilesList().IsEmpty())
	{
		return TileProfundityMap[current_tile] + 1;
	}
	
	TArray<ATile*>& NearbyTiles = current_tile->GetNearbyTilesList();
	
	TArray<bool> visited;
	visited.Init(false, NearbyTiles.Num());
	int32 end_control = 0;
	
	std::uniform_int_distribution<int> distribution(0, NearbyTiles.Num() - 1);
	
	while (end_control != NearbyTiles.Num())
	{
		int32 randomNearbyTileListIndex = distribution(generator);

		if (visited[randomNearbyTileListIndex] != true)
		{
			visited[randomNearbyTileListIndex] = true;
			end_control++;

			ATile* next_tile = NearbyTiles[randomNearbyTileListIndex];
			int32* nextTileIndexPtr = TileIndexMap.Find(next_tile);
			if (!nextTileIndexPtr) continue;
			int32 nextTileIndex = *nextTileIndexPtr;

			if (Colors[nextTileIndex] == EDFSColor::WHITE)
			{
				int32 new_profundity = Labirinth(next_tile, Colors, generator);
				if (new_profundity > TileProfundityMap[current_tile])
				{
					TileProfundityMap[current_tile] = new_profundity;
				}
			}
		}
	}
	Colors[TileIndex] = EDFSColor::BLACK;
	return TileProfundityMap[current_tile] + 1;
}

void AGameField::GenerateObstacle()
{
	int32 ObstaclesNumber = StaticCast<int32>(TilesArray.Num() * ObstaclesPercentage);
	int32 control_value = 0;
	for (auto& tile : TilesArray)
	{
		if (tile->GetTileStatus() == ETileStatus::OCCUPIED)
		{
			control_value++;
		}
	}

	TArray<ATile*> ProfunditySorted_Array = TilesArray;

	std::sort(ProfunditySorted_Array.GetData(), ProfunditySorted_Array.GetData() + ProfunditySorted_Array.Num(), 
		[this](const ATile* a, const ATile* b) 
		{
			if (TileProfundityMap.Contains(a) && TileProfundityMap.Contains(b))
			{
				return TileProfundityMap[a] < TileProfundityMap[b];
			}
			return false;
		}
	);

	for (auto& tile : ProfunditySorted_Array)
	{
		if (tile->GetTileStatus() == ETileStatus::EMPTY && control_value < ObstaclesNumber)
		{
			tile->SetTileStatus(ETileStatus::OCCUPIED);
			control_value++;
			FVector LocationTile = tile->GetActorLocation();
			FVector Location(LocationTile.X, LocationTile.Y, (LocationTile.Z + (tile->GetStaticMeshScale().Z * 100.f)));
			AObstacle* Obj = GetWorld()->SpawnActor<AObstacle>(ObstacleClass, Location, FRotator::ZeroRotator);
			Obj->SetActorScale3D(FVector(tile->GetActorScale3D().X, tile->GetActorScale3D().Y, 1));
			Obj->SetReferenceTile(tile);
			ObstaclesArray.Add(Obj);
		}
		if (control_value >= ObstaclesNumber && tile->GetTileStatus() == ETileStatus::EMPTY)
		{
			FreeTilesStartArray.Add(tile);
		}
	}
}

/*---------------------------------------------------------------------------*/

const float AGameField::GetTileDimension()
{
	return TileDimension;
}

const float AGameField::GetTilePadding()
{
	return TilePadding;
}

TArray<ATile*>& AGameField::GetFreeTilesStartArray()
{
	return FreeTilesStartArray;
}

void AGameField::SetFieldSize(const int32 addFieldSize)
{
	FieldSize = addFieldSize;
}

void AGameField::SetObstaclesPercentage(const float addObstaclesPercentage)
{
	ObstaclesPercentage = addObstaclesPercentage;
}

// Called every frame
//void AGameField::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

