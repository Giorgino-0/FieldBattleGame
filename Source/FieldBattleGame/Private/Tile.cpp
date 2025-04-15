// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "Minifigure.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//template function that create a component
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	//define ROOT component
	SetRootComponent(Scene);

	//Connect the StaticMeshComponent as an attachement to Scene
	StaticMeshComponent->SetupAttachment(Scene);

	//default values for variables
	TileStatus = ETileStatus::EMPTY;
	TileOwner = ETileOwner::FREE;
	TileGridPosition = FVector2D(0, 0);
	Occupant = nullptr;
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
	if (StaticMeshComponent)
	{
		//to get the material that was assigned from blueprint
		UMaterialInterface* BaseMaterial = StaticMeshComponent->GetMaterial(0);

		//I create a dynamic material copy and I set it as the current material
		if (BaseMaterial)
		{
			DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
			StaticMeshComponent->SetMaterial(0, DynamicMaterial);

			FLinearColor ColorValue;
			if (DynamicMaterial->GetVectorParameterValue(FName("Color"), ColorValue))
			{
				OriginalColor = ColorValue;
			}
		}

	}
}

/*---------------------------------------------------------------------------*/

void ATile::SetTileGridPosition(const int32 X_value, const int32 Y_value)
{
	TileGridPosition.Set(X_value, Y_value);
}

void ATile::SetTileStatus(const ETileStatus addTileStatus)
{
	TileStatus = addTileStatus;
}

void ATile::SetTileOwner(const ETileOwner addTileOwner)
{
	TileOwner = addTileOwner;
}

void ATile::SetOccupant(AMinifigure* addOccupant)
{
	Occupant = addOccupant;
}

TArray<ATile*>& ATile::GetNearbyTilesList()
{
	return NearbyTilesList;
}

const FVector2D ATile::GetTileGridPosition()
{
	return TileGridPosition;;
}

const ETileStatus ATile::GetTileStatus()
{
	return TileStatus;
}

const ETileOwner ATile::GetTileOwner()
{
	return TileOwner;
}

AMinifigure* ATile::GetOccupant()
{
	return Occupant;
}

const FVector ATile::GetStaticMeshScale()
{
	return StaticMeshComponent->GetComponentScale();
}

//To highlight the tile if a minifigure can move on it
void ATile::HighlightGreen()
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue("Color", FLinearColor::Green);
	}
}
//to unhighlight the tile when is not selected anymore
void ATile::Unhighlight()
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue("Color", OriginalColor);
	}
}


// Called every frame
//void ATile::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

