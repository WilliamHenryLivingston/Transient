// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "FindItemAction.h"

#include "Kismet/GameplayStatics.h"

#include "MoveToPointAction.h"
#include "EquipItemAction.h"

CFindItemAction* CFindItemAction::FindInWorldOrFail(
    AUnitAgent* Owner, TSubclassOf<AItemActor> TargetType,
    float MaxDistance, bool EquipAfter
) {
    CFindItemAction* Attempt = new CFindItemAction(TargetType, MaxDistance, false, EquipAfter);
    
    AItemActor* FoundTarget = Attempt->FindTargetInWorld(Owner);
    if (FoundTarget == nullptr) {
        delete Attempt;
        return nullptr;
    }

    Attempt->Target = FoundTarget;
    Attempt->FindAttempted = true;

    return Attempt;
}

CFindItemAction::CFindItemAction(
    TSubclassOf<AItemActor> InitTargetType,
    float InitMaxDistance, bool InitAllowSpawn, bool EquipAfter
) {
    this->TargetType = TargetType;
    this->MaxDistance = InitMaxDistance;
    this->AllowSpawn = InitAllowSpawn;

    this->Target = nullptr;
    this->FindAttempted = false;

    this->Started = false;
    this->TakeStarted = false;
    this->EquipStarted = !EquipAfter;

#if DEBUG_ACTIONS
    FString Name = TEXT("worldfind <spawn>");
    if (this->Target != nullptr) Name = FString::Printf(TEXT("worldfind %s"), *this->Target->GetName());
    this->DebugInfo = Name;
#endif
}

CFindItemAction::~CFindItemAction() {}

FActionTickResult CFindItemAction::ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) {
    if (Owner->UnitArmsOccupied()) return FActionTickResult::Unfinished;

    if (!this->Started) {
        this->Started = true;

        if (!this->FindAttempted) {
            this->Target = this->FindTargetInWorld(Owner);
            this->FindAttempted = true;
        }

        if (this->Target == nullptr) {
            if (!this->AllowSpawn) return FActionTickResult::Error(0);

            this->Target = Owner->GetWorld()->SpawnActor<AItemActor>(
                this->TargetType,
                Owner->GetActorLocation(),
                Owner->GetActorRotation(),
                FActorSpawnParameters()
            );
        }
        else {
            return FActionTickResult::UnfinishedAnd(new CMoveToPointAction(
                this->Target->GetActorLocation(), Owner->TakeReach * 0.75f, false
            ));
        }
    }

    if (!this->TakeStarted) {
        // Ensure still available, restart if not.
        if (this->Target->ItemHolder() != nullptr) {
            this->Target = nullptr;
            this->FindAttempted = false;
            this->Started = false;
            return FActionTickResult::Unfinished;
        }

        this->TakeStarted = true;
        Owner->UnitTakeItem(this->Target);
        return FActionTickResult::Unfinished;
    }

    if (!this->EquipStarted) {
        this->EquipStarted = true;
        return FActionTickResult::UnfinishedAnd(new CEquipItemAction(this->Target));
    }

    return FActionTickResult::Finished;
}

// TODO: Faster approach.
AItemActor* CFindItemAction::FindTargetInWorld(AUnitAgent* Owner) {
    TArray<AActor*> Options;
    UGameplayStatics::GetAllActorsOfClass(Owner->GetWorld(), this->TargetType, Options);

    FVector OwnerLocation = Owner->GetActorLocation();

    AItemActor* Best = nullptr;
    float BestDistance = this->MaxDistance;
    for (int i = 0; i < Options.Num(); i++) {
        AItemActor* Check = Cast<AItemActor>(Options[i]);
        if (Check->ItemHolder() != nullptr) continue;
        
        float CheckDistance = (OwnerLocation - Check->GetActorLocation()).Size();
        if (CheckDistance < BestDistance) {
            BestDistance = CheckDistance;
            Best = Check;
        }
    }

    return Best;
}
