// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/WeakObjectPtr.h"
#include "Tile.generated.h"

class AMinifigure;

UENUM()
enum class ETileStatus : uint8
{
	EMPTY UMETA(DisplayName="Empty"),
	OCCUPIED UMETA(DisplayName="Occupied"),
};
UENUM()
enum class ETileOwner : uint8
{
	FREE	UMETA(DisplayName = "Free"),
	BLUE	UMETA(DisplayName = "Blue"),
	RED	UMETA(DisplayName = "Red"),
};

UCLASS()
class FIELDBATTLEGAME_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	//Default member
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;
	
	//member to be able to change the color and turn back to the original 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FLinearColor OriginalColor;

	//other parameters
	//position of the tile inside the grid
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D TileGridPosition;

	//list of nearby tiles
	UPROPERTY(Transient)
	TArray<ATile*> NearbyTilesList;

	//status of the Tile: empty --> with nothing ; occupied --> an obstacle or a minifigure
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ETileStatus TileStatus;

	//Owner of the Tile: free --> with nothing or an obstacle; color -> team that control the tile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ETileOwner TileOwner;

	//the minifigure that is on the tile --> if there is one
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AMinifigure* Occupant;


public:
	//this parameter is public for an easy access in other part of the program
	TArray<ATile*>& GetNearbyTilesList();

public:

	void SetTileGridPosition(const int32 X_value, const int32 Y_value);

	void SetTileStatus(const ETileStatus addTileStatus);

	void SetTileOwner(const ETileOwner addTileOwner);

	void SetOccupant(AMinifigure* addOccupant);

	const FVector2D GetTileGridPosition();

	const ETileStatus GetTileStatus();

	const ETileOwner GetTileOwner();

	AMinifigure* GetOccupant();

	const FVector GetStaticMeshScale();

	void HighlightGreen();

	void Unhighlight();

//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;
};
