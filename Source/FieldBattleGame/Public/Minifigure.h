// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "Minifigure.generated.h"

class AG_GameMode;

DECLARE_DELEGATE_OneParam(FMinifigureDestroyedDelegate, AMinifigure*);

UENUM()
enum class ETeamColor : uint8
{
	BLUE	UMETA(DisplayName = "Blue"),
	RED	UMETA(DisplayName = "Red"),
};
UENUM()
enum class EAttackType : uint8
{
	MELEE UMETA(DisplayName = "Melee"),
	RANGED UMETA(DisplayName = "Ranged"),
};

UENUM() 
enum class EMinifigureType : uint8
{
	BRAWLER UMETA(DIsplayName = "Brawler"),
	SNIPER UMETA(Displayname = "Sniper"),
};

USTRUCT(BlueprintType)
struct FMaterialSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	TArray<UMaterialInterface*> MaterialList;
};

UCLASS(Abstract, Blueprintable)
class FIELDBATTLEGAME_API AMinifigure : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMinifigure();
	AG_GameMode* GameMode;
protected:
	//Default member
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	TMap<ETeamColor,FMaterialSet> MaterialMap;

	//Normal Parameter
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	ETeamColor TeamColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EMinifigureType MinifigureType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ATile* CurrentPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttackType AttackType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinDamage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanCounter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeCountered;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinCounterDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxCounterDamage;
public:
	bool bCanPlay;
	FMinifigureDestroyedDelegate OnMinifigureDestroyed;
public:
	void SetMaterialByTeamColor();
public:
	void SetTeamColor(ETeamColor addTeamColor);
	void SetCurrentPosition(ATile* addCurrenPosition);
	ATile* GetCurrentPosition();
	const ETeamColor GetTeamColor();
	const int32 GetMaxMovement();
	const int32 GetAttackRange();
	const EMinifigureType GetMinifigureType();
public:
	TArray<AMinifigure*> GetAttackTarget();
	TMap<ATile*, ATile*> GetMovementTarget();
	void MoveTo(TMap<ATile*, ATile*> MovementTarget, ATile* TargetTile);
	void Attack(AMinifigure* Target);
protected:
	void StartMovement(TArray<ATile*> Sequence);
	void Movement(TArray<ATile*> Sequence, int32 StepIndex);
	virtual bool Counter(AMinifigure* CounterTarget) { return false; };
	void DestroyAndNotifyPlayer();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SetMinifigureType();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMaterialInstanceDynamic* DynamicMaterial;

protected:
	UPROPERTY()
	FLinearColor OriginalColor;

public:
	void HighlightRed()
	{
		if (DynamicMaterial)
		{
			DynamicMaterial->SetVectorParameterValue("BackGroundColor", FLinearColor::Red);
			DynamicMaterial->SetScalarParameterValue("GlowScale", 50.f);
		}

	}

	void Unhighlight()
	{
		if (DynamicMaterial)
		{
			DynamicMaterial->SetVectorParameterValue("BackGroundColor", OriginalColor);
			DynamicMaterial->SetScalarParameterValue("GlowScale", 1.f);
		}
	}
//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;

};
