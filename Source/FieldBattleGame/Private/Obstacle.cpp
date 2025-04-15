// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle.h"

// Sets default values
AObstacle::AObstacle()
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
}

// Called when an instance of this class is placed (in editor) or spawned
void AObstacle::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (MaterialList.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, MaterialList.Num() - 1);
		StaticMeshComponent->SetMaterial(0, MaterialList[RandomIndex]);
	}
}

// Called when the game starts or when spawned
void AObstacle::BeginPlay()
{
	Super::BeginPlay();

}

/*---------------------------------------------------------------------------*/

ATile* AObstacle::GetReferenceTile()
{
	return ReferenceTile;
}

void AObstacle::SetReferenceTile(ATile* addReferenceTile)
{
	ReferenceTile = addReferenceTile;
}

// Called every frame
//void AObstacle::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

