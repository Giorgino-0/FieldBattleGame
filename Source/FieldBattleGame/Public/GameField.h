// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "Obstacle.h"
#include "random"
#include "GameField.generated.h"

UENUM()
enum class EDFSColor : uint8
{
	WHITE	UMETA(DisplayName = "White"),
	GREY	UMETA(DisplayName = "Grey"),
	BLACK	UMETA(DisplayName = "Black"),
};


UCLASS()
class FIELDBATTLEGAME_API AGameField : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameField();

protected:
	// Called when an instance of this class is placed (in editor) or spawned
	virtual void OnConstruction(const FTransform& Transform) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	//array of Obstacles
	UPROPERTY(Transient)
	TArray<AObstacle*> ObstaclesArray;

	//subclass of obstacles to generate with control
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AObstacle> ObstacleClass;

	//array of Tiles
	UPROPERTY(Transient)
	TArray<ATile*> TilesArray;

	//subclass of tiles to generate with control
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATile> TilesClass;

	//mapping between positions and Tiles
	UPROPERTY(Transient)
	TMap<FVector2D, ATile*> TilesMap;

	//mapping between tiles and profundity in the labirinth generation
	UPROPERTY(Transient)
	TMap<ATile*, int32> TileProfundityMap;

	//mapping between tiles and the index in the labirinth generation
	UPROPERTY(Transient)
	TMap<ATile*, int32>TileIndexMap;

	//array of freetiles after the obstacle generation
	UPROPERTY(Transient)
	TArray<ATile*> FreeTilesStartArray;

	//size of the field generated
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 FieldSize;

	//Tile dimension
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TileDimension;

	//Tile padding
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TilePadding;

	//Real next tile position
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float NextCellPositionMultiplier;

	//Percentage of obstacles
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ObstaclesPercentage;


public:
	//function to call to generate and destroy the field
	void CreateGameField();

	void DestroyField();

protected:
	//convert from position X,Y to the real position
	FVector GetPositionFromXY(const int32 X_value, const int32 Y_value);

	//generate an empty field
	void GenerateEmptyField();

	//fill the nearby tiles list inside the tiles of the GameField
	void Fill_NearbyTilesList();

	//function to set the parameters for the creation of the "tree" that I use to generate obstacles --> it's the setting of a DFS algorithm
	//it's called Labirinth becouse it's the same method used to generate Labirinths
	void LabirinthGenerator();

	//function that create the "tree"
	int Labirinth(ATile* current_tile, TArray<EDFSColor>& Colors, std::mt19937& generator);

	//generate the obstacle
	void GenerateObstacle();

public:
	//access method to the parameters
	const float GetTileDimension();
	const float GetTilePadding();
	TArray<ATile*>& GetFreeTilesStartArray();
	void SetFieldSize(const int32 addFieldSize);
	void SetObstaclesPercentage(const float addObstaclesPercentage);


//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;

};
