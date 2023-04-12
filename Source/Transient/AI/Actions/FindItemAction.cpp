// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "FindItemAction.h"

#include "Kismet/GameplayStatics.h"

#include "../AIUnit.h"
#include "MoveToPointAction.h"

CFindItemAction* CFindItemAction::AIActionTryPlan(AActor* Owner, TSubclassOf<AItemActor> TargetType, float MaxDistance) {
    TArray<AActor*> Options;
    UGameplayStatics::GetAllActorsOfClass(Owner->GetWorld(), TargetType->StaticClass(), Options);
    
    FVector OwnerLocation = Owner->GetActorLocation();

    // TODO: Faster approach.
    AActor* Best = nullptr;
    float BestDistance = MaxDistance;
    for (int i = 0; i < Options.Num(); i++) {
        AActor* Check = Options[i];
        
        float CheckDistance = (OwnerLocation - Check->GetActorLocation()).Size();
        if (CheckDistance < BestDistance) {
            BestDistance = CheckDistance;
            Best = Check;
        }
    }

    if (Best == nullptr) return nullptr;

    return new CFindItemAction(Cast<AItemActor>(Best));
}

CFindItemAction::CFindItemAction(AItemActor* InitTarget) {
    this->Target = InitTarget;
    this->TravelStarted = false;
}

CFindItemAction::~CFindItemAction() {}

FAIActionTickResult CFindItemAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    if (!this->TravelStarted) {
        this->TravelStarted = true;
        return FAIActionTickResult(false, new CMoveToPointAction(this->Target, Owner->TakeReach * 0.75f));
    }

    Owner->UnitTakeItem(Target);
    return this->Finished;
}
