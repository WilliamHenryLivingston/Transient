// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "UseItemAction.h"

#include "Kismet/KismetMathLibrary.h"

#include "Transient/Items/WeaponItem.h"

#include "EquipItemAction.h"

#define TARGETED_SETUP_TIME 1.0f

CUseItemAction::CUseItemAction(AItemActor* InitItem, AActor* InitUseTarget) {
    this->Item = InitItem;
    this->UseTarget = InitUseTarget;

    this->SetupTimer = 0.0f;
    this->UseStarted = false;

#if DEBUG_ACTIONS
    FString Name = TEXT("<current>");
    if (this->Item != nullptr) Name = this->Item->GetName();
    this->DebugInfo = FString::Printf(TEXT("use %s"), *Name);
#endif
}

CUseItemAction::~CUseItemAction() {}

FActionTickResult CUseItemAction::ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) {
    if (this->Item == nullptr) {
        this->Item = Owner->UnitActiveItem();

        if (this->Item == nullptr) return FActionTickResult::Error(0);
    }

    if (Owner->UnitArmsOccupied()) return this->Unfinished;

    if (Owner->UnitActiveItem() != this->Item) {
        return FActionTickResult::UnfinishedAnd(new CEquipItemAction(this->Item));
    }

    if (this->UseTarget != nullptr && this->SetupTimer < TARGETED_SETUP_TIME) {
        this->SetupTimer += DeltaTime;

        FVector TargetLocation = this->UseTarget->GetActorLocation();
        Owner->UnitTorsoPitchTowards(TargetLocation);
        Owner->UnitFaceTowards(TargetLocation);
        return FActionTickResult::Unfinished;
    }

    if (!this->UseStarted) {
        if (this->Item->IsA(AWeaponItem::StaticClass())) Owner->UnitReloadActiveWeapon();
        else Owner->UnitUseActiveItem(this->UseTarget);

        this->UseStarted = true;
        return FActionTickResult::Unfinished;
    }
    
    Owner->UnitSetTorsoPitch(0.0f);
    return FActionTickResult::Finished;
}
