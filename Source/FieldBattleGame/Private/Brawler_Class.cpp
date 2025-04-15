// Fill out your copyright notice in the Description page of Project Settings.


#include "Brawler_Class.h"
#include "Sniper_Class.h"
// Sets default values
ABrawler_Class::ABrawler_Class()
{
	//template function that create a component
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	//define ROOT component
	SetRootComponent(Scene);

	//Connect the StaticMeshComponent as an attachement to Scene
	StaticMeshComponent->SetupAttachment(Scene);

	MaxMovement = 6;
	AttackRange = 1;
	AttackType = EAttackType::MELEE;
	MaxHealth = 40;
	CurrentHealth = 40;
	bCanCounter = true;
	bCanBeCountered = false;
	MinDamage = 1;
	MaxDamage = 6;
	MinCounterDamage = 1;
	MaxCounterDamage = 3;
}

bool ABrawler_Class::Counter(AMinifigure* CounterTarget)
{
	if (CounterTarget->IsA<ASniper_Class>())
	{
		return GetCurrentPosition()->GetNearbyTilesList().Contains(CounterTarget->GetCurrentPosition());
	}
	return false;
}

