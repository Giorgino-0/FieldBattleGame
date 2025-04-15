// Fill out your copyright notice in the Description page of Project Settings.


#include "Sniper_Class.h"

// Sets default values
ASniper_Class::ASniper_Class()
{
	//template function that create a component
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	//define ROOT component
	SetRootComponent(Scene);

	//Connect the StaticMeshComponent as an attachement to Scene
	StaticMeshComponent->SetupAttachment(Scene);

	MaxMovement = 3;
	AttackRange = 10;
	AttackType = EAttackType::RANGED;
	MaxHealth = 20;
	CurrentHealth = 20;
	bCanCounter = true;
	bCanBeCountered = true;
	MinDamage = 4;
	MaxDamage = 8;
	MinCounterDamage = 1;
	MaxCounterDamage = 3;
}

bool ASniper_Class::Counter(AMinifigure* CounterTarget)
{
	if (CounterTarget->IsA<ASniper_Class>())
	{
		return true;
	}
	return false;
}

