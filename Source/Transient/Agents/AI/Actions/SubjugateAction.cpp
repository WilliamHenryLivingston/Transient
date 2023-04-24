// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "SubjugateAction.h"

#include "Transient/Items/WeaponItem.h"

#include "MoveToPointAction.h"
#include "EquipItemAction.h"
#include "UseItemAction.h"

#define HURT_MOTION_TIME = 2.0f;
#define HURT_FIRE_TIME = 0.5f;
#define HURT_DIST = 200.0f;

CSubjugateAction::CSubjugateAction(AAgentActor* InitTarget) {
    this->Target = InitTarget;

    this->TravelStarted = false;
    this->HurtTimer = 0.0f;

#if DEBUG_AI
    FString Name = TEXT("<missing>");
    if (this->Target != nullptr) Name = this->Target->GetName();
    this->DebugInfo = FString::Printf(TEXT("subjugate %s"), *Name);
#endif
}

CSubjugateAction::~CSubjugateAction() {}

FActionTickResult CSubjugateAction::ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) {
    if (!IsValid(this->Target)) return FActionTickResult::Finished;

    if (Owner->UnitArmsOccupied()) return FActionTickResult::Unfinished;

    if (Owner->UnitActiveWeapon() == nullptr) {
        AWeaponItem* Weapon = Owner->UnitInventory()->InventoryFindBestItemOfClass(AWeaponItem::StaticClass());
        if (Weapon == nullptr) return FActionTickResult::Error(0);

        return FActionTickResult::UnfinishedAnd(new CEquipItemAction(Weapon));
    }

    if (!this->TravelStarted) {
        this->TravelStarted = true;

        return FActionTickResult::UnfinishedAnd(new CMoveToPointAction(this->Target, HURT_DIST, false));
    }

    if (this->HurtTimer < HURT_MOTION_TIME) {
        this->HurtTimer += DeltaTime;
        Owner->UnitFaceTowards(this->Target->GetActorLocation());
        Owner->UnitSetTriggerPulled(this->HurtTimer > HURT_MOTION_TIME - HURT_FIRE_TIME);

        return FActionTickResult::Unfinished;
    }

    Owner->UnitSetTriggerPulled(false);
    return FActionTickResult::FinishedAnd(new CUseItemAction(nullptr));
}
