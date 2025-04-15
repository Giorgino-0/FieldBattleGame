// Fill out your copyright notice in the Description page of Project Settings.


#include "Minifigure.h"
#include "G_GameMode.h"


// Sets default values
AMinifigure::AMinifigure()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AMinifigure::BeginPlay()
{
    Super::BeginPlay();

    if (StaticMeshComponent)
    {
        // Ottieni il materiale assegnato via Blueprint
        UMaterialInterface* BaseMaterial = StaticMeshComponent->GetMaterial(0);

        // Crea una copia dinamica da quel materiale
        if (BaseMaterial)
        {
            DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
            StaticMeshComponent->SetMaterial(0, DynamicMaterial);

            FLinearColor ColorValue;
            if (DynamicMaterial->GetVectorParameterValue(FName("BackGroundColor"), ColorValue))
            {
                OriginalColor = ColorValue;
            }
        }

    }
    GameMode = Cast<AG_GameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("No Gamemode founded"));
        return;
    }
    SetMinifigureType();
}


void AMinifigure::SetMinifigureType()
{
    if (this->GetClass()->GetName().Contains("Sniper")) MinifigureType = EMinifigureType::SNIPER;
    else if (this->GetClass()->GetName().Contains("Brawler")) MinifigureType = EMinifigureType::BRAWLER;
}

void AMinifigure::SetMaterialByTeamColor()
{
    if (MaterialMap.Contains(TeamColor))
    {
        FMaterialSet* MaterialSet = MaterialMap.Find(TeamColor);

        if (MaterialSet && MaterialSet->MaterialList.Num() > 0)
        {
            // Scegli un materiale casuale dall'array
            int32 RandomIndex = FMath::RandRange(0, MaterialSet->MaterialList.Num() - 1);
            UMaterialInterface* SelectedMaterial = MaterialSet->MaterialList[RandomIndex];

            // Applica il materiale alla mesh
            StaticMeshComponent->SetMaterial(0, SelectedMaterial);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Nessun materiale per TeamID %d!"), TeamColor);
    }
}

void AMinifigure::SetTeamColor(ETeamColor addTeamColor)
{
    TeamColor = addTeamColor;
}

void AMinifigure::SetCurrentPosition(ATile* addCurrenPosition)
{
    CurrentPosition = addCurrenPosition;
}

ATile* AMinifigure::GetCurrentPosition()
{
    return CurrentPosition;
}

const ETeamColor AMinifigure::GetTeamColor()
{
    return TeamColor;
}

const int32 AMinifigure::GetMaxMovement()
{
    return MaxMovement;
}

const int32 AMinifigure::GetAttackRange()
{
    return AttackRange;
}
const EMinifigureType AMinifigure::GetMinifigureType()
{
    return MinifigureType;
}

TArray<AMinifigure*> AMinifigure::GetAttackTarget()
{
    
    TQueue<ATile*> Queue;

    TMap<ATile*, int32> Distanza;
    
    TArray<AMinifigure*> Targets;
    
    Queue.Enqueue(CurrentPosition);
    if (!IsValid(CurrentPosition))
    {
        UE_LOG(LogTemp, Error, TEXT("CurrentPosition non è valido!"));
    }
    Distanza.Add(CurrentPosition,0);
    while (!Queue.IsEmpty())
    {
        ATile* tile = nullptr;
        Queue.Dequeue(tile);

        if (Distanza[tile] >= GetAttackRange()) continue;

        for (auto& neighbor : tile->GetNearbyTilesList())
        {
            if (Distanza.Find(neighbor) == nullptr)
            {
                Distanza.Add(neighbor, Distanza[tile] + 1);
                Queue.Enqueue(neighbor);
                if ((neighbor->GetTileOwner() == ETileOwner::RED && GetTeamColor() == ETeamColor::BLUE) || (neighbor->GetTileOwner() == ETileOwner::BLUE && GetTeamColor() == ETeamColor::RED))
                {
                    Targets.Add(neighbor->GetOccupant());
                }
            }
        }
    }
    return Targets;
}

void AMinifigure::Attack(AMinifigure* Target)
{
    
    int32 Damage = FMath::RandRange(MinDamage, MaxDamage);
    Target->CurrentHealth = Target->CurrentHealth - Damage;
    GameMode->LogAttackMinifigure(this, Target, Damage, false);
    if (bCanBeCountered)
    {
        if (Target->bCanCounter && Target->Counter(this))
        {
            
            int32 CounterDamage = FMath::RandRange(MinCounterDamage, MaxCounterDamage);
            CurrentHealth = CurrentHealth - CounterDamage;
            GameMode->LogAttackMinifigure(Target, this, CounterDamage, true);
            if (CurrentHealth <= 0)
            {
                GameMode->LogMinifigureDeath(this);
                ATile* Tile = GetCurrentPosition();
                Tile->SetTileStatus(ETileStatus::EMPTY);
                Tile->SetTileOwner(ETileOwner::FREE);
                Tile->SetOccupant(nullptr);
                DestroyAndNotifyPlayer();
            }
        }
    }
    if (Target->CurrentHealth <= 0)
    {
        GameMode->LogMinifigureDeath(Target);
        ATile* Tile = Target->GetCurrentPosition();
        Tile->SetTileStatus(ETileStatus::EMPTY);
        Tile->SetTileOwner(ETileOwner::FREE);
        Tile->SetOccupant(nullptr);
        Target->DestroyAndNotifyPlayer();
    }
    FTimerHandle AttackTimer;
    GetWorld()->GetTimerManager().SetTimer(AttackTimer, GameMode, &AG_GameMode::ControlEnd, 2.0f, false);
}

TMap<ATile*, ATile*> AMinifigure::GetMovementTarget()
{
    TQueue<ATile*> Queue;

    TMap<ATile*, int32> Distanza;

    TMap<ATile*, ATile*> Nodo_Precedente;

    Queue.Enqueue(CurrentPosition);
    if (!IsValid(CurrentPosition))
    {
        UE_LOG(LogTemp, Error, TEXT("CurrentPosition non è valido!"));
    }
    Distanza.Add(CurrentPosition,0);
    Nodo_Precedente.Add(CurrentPosition, nullptr);
    while (!Queue.IsEmpty())
    {
        ATile* tile = nullptr;
        Queue.Dequeue(tile);

        if (Distanza[tile] >= GetMaxMovement()) continue;

        for (auto& neighbor : tile->GetNearbyTilesList())
        {
            if (Distanza.Find(neighbor) == nullptr && neighbor->GetTileStatus() != ETileStatus::OCCUPIED)
            {
                Distanza.Add(neighbor, Distanza[tile] + 1);
                Nodo_Precedente.Add(neighbor, tile);
                Queue.Enqueue(neighbor);
            }
        }
    }
    Nodo_Precedente.Remove(CurrentPosition);
    return Nodo_Precedente;
}

void AMinifigure::MoveTo(TMap<ATile*, ATile*> MovementTargets, ATile* TargetTile)
{
    GameMode->LogMoveToTile(this, TargetTile);
    TArray<ATile*> Sequence = {};
    while (TargetTile != CurrentPosition)
    {
        Sequence.Add(TargetTile);
        TargetTile = MovementTargets.FindRef(TargetTile);
    }
    StartMovement(Sequence);
}

void AMinifigure::StartMovement(TArray<ATile*> Sequence)
{
    int32 StepIndex = Sequence.Num() - 1;
    ATile* Tile = GetCurrentPosition();
    Tile->SetTileStatus(ETileStatus::EMPTY);
    Tile->SetTileOwner(ETileOwner::FREE);
    Tile->SetOccupant(nullptr);
    Movement(Sequence, StepIndex);
}

void AMinifigure::Movement(TArray<ATile*> TimerSequence, int32 StepIndex) {

    if (StepIndex >= 0) {
        FVector NewTileLocation = TimerSequence[StepIndex]->GetActorLocation();
        FVector NewLocation(NewTileLocation.X, NewTileLocation.Y, (NewTileLocation.Z + (TimerSequence[StepIndex]->GetStaticMeshScale().Z * 100.f)));
        SetActorLocation(NewLocation);
        SetCurrentPosition(TimerSequence[StepIndex]);
        StepIndex--;
        FTimerHandle Timer;
        GetWorld()->GetTimerManager().SetTimer(Timer, [this, TimerSequence, StepIndex]()
            {
                Movement(TimerSequence, StepIndex);
            }, 0.3f, false);
    }
    else
    {   

        GetCurrentPosition()->SetOccupant(this);
        GetCurrentPosition()->SetTileStatus(ETileStatus::OCCUPIED);
        if (GetTeamColor() == ETeamColor::BLUE) 
        {
            GetCurrentPosition()->SetTileOwner(ETileOwner::BLUE);
        }
        else
        {
            GetCurrentPosition()->SetTileOwner(ETileOwner::RED);
        }
        GameMode->NowAttack(this);
    }
}

void AMinifigure::DestroyAndNotifyPlayer()
{
    if (OnMinifigureDestroyed.IsBound()) {
        OnMinifigureDestroyed.Execute(this);
    }
    // Distruggere la minifigura
    Destroy();
}




//// Called every frame
//void AMinifigure::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

