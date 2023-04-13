// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "FindItemAction.h"

#include "Kismet/GameplayStatics.h"

#include "../AIUnit.h"
#include "MoveToPointAction.h"
#include "EquipItemAction.h"

CFindItemAction::CFindItemAction(AActor* Owner, TSubclassOf<AItemActor> TargetType, float MaxDistance, bool EquipAfter) {
    TArray<AActor*> Options;
    UGameplayStatics::GetAllActorsOfClass(Owner->GetWorld(), TargetType, Options);

    FVector OwnerLocation = Owner->GetActorLocation();

    // TODO: Faster approach.
    AActor* Best = nullptr;
    float BestDistance = MaxDistance;
    for (int i = 0; i < Options.Num(); i++) {
        AActor* Check = Options[i];
        if (Cast<AItemActor>(Check)->CurrentHolder != nullptr) continue;
        
        float CheckDistance = (OwnerLocation - Check->GetActorLocation()).Size();
        if (CheckDistance < BestDistance) {
            BestDistance = CheckDistance;
            Best = Check;
        }
    }

    this->TargetType = TargetType;
    this->Target = Cast<AItemActor>(Best);
    this->EquipStarted = !EquipAfter;
    this->Started = false;
    this->TakeStarted = false;

    FString Name = TEXT("worldfind <spawn>");
    if (this->Target != nullptr) {
        Name = FString::Printf(TEXT("worldfind %s"), *this->Target->GetName());
    }
    this->DebugInfo = Name;
}

CFindItemAction::~CFindItemAction() {}

FAIActionTickResult CFindItemAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    if (Owner->UnitAreArmsOccupied()) return this->Unfinished;

    if (!this->Started) {
        this->Started = true;

        if (this->Target == nullptr) {
            this->Target = Owner->GetWorld()->SpawnActor<AItemActor>(
                this->TargetType,
                Owner->GetActorLocation(),
                Owner->GetActorRotation(),
                FActorSpawnParameters()
            );
        }
        else {
            return FAIActionTickResult(false, new CMoveToPointAction(this->Target, Owner->TakeReach * 0.75f));
        }
    }

    if (!this->TakeStarted) {
        this->TakeStarted = true;

        // Someone else got there first.
        if (this->Target->CurrentHolder != nullptr) return this->Finished;
        Owner->UnitTakeItem(this->Target);
        return this->Unfinished;
    }

    if (!this->EquipStarted) {
        this->EquipStarted = true;

        return FAIActionTickResult(false, new CEquipItemAction(this->Target));
    }

    return this->Finished;
}
