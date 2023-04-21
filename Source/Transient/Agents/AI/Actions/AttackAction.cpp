// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "AttackAction.h"

#include "Kismet/KismetMathLibrary.h"

#include "Transient/Common.h"
#include "Transient/Items/WeaponItem.h"
#include "Transient/Agents/InteractiveAgent.h"
#include "Transient/Agents/Units/AIUnit.h"
#include "Transient/Agents/AgentManager.h"

#include "AttackStanceAction.h"
#include "MoveToPointAction.h"
#include "FindItemAction.h"
#include "InteractAction.h"
#include "EquipItemAction.h"
#include "UseItemAction.h"
#include "MultiAction.h"
#include "WaitAction.h"

#define DIRECT_ATTACK_DIST_COEF 0.5f

CAttackAction::CAttackAction() {
#if DEBUG_ACTIONS
    this->DebugInfo = TEXT("attacks");
#endif
}

FActionParentTickResult CAttackAction::ActionParentTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) {
    AAgentActor* Target = this->ActionCurrentTarget(Owner);

    if (Target == nullptr) {
        Owner->UnitSetTriggerPulled(false);

        return FActionParentTickResult::Stop;
    }

    this->ActionHandleTriggerAndFacing(Owner);

    bool IsHealing = (
        State->StateRead(SK_ENERGY_HEAL) == 0 ||
        State->StateRead(SK_KINETIC_HEAL) == 0
    );
    if (IsHealing) return FActionParentTickResult::Continue;

    CAction* WeaponTask = this->ActionHandleWeapon(RawOwner);
    if (WeaponTask != nullptr) return FActionParentTickResult::ContinueAnd(WeaponTask);

    return FActionParentTickResult::Continue;
}

FActionTickResult CAttackAction::ActionTick(AUnitAgent* Owner, CAIState* State, float DeltaTime) {
    AAgentManager* Manager = AAgentManager::AgentsGetManager(Owner->GetWorld());
    Manager->UnitAIUnclaimAllNavNodesFor(Owner);

    AAgentActor* Target = this->ActionCurrentTarget(Owner);
    if (Target == nullptr) {
        Owner->UnitSetTriggerPulled(false);
        return FActionTickResult::Finished;
    }

    CAction* WeaponTask = this->ActionHandleWeapon(Owner);
    if (WeaponTask != nullptr) return FActionTickResult::UnfinishedAnd(WeaponTask);

    AWeaponItem* Weapon = Owner->UnitActiveWeapon();
    if (Weapon == nullptr) return FActionTickResult::Error(0);

    this->ActionHandleTriggerAndFacing(Owner);

    FVector TargetLocation = Target->GetActorLocation();

    float DirectAttackRadius = Weapon->AIFireDistance * DIRECT_ATTACK_DIST_COEF;
    bool DirectAttack = (
        (TargetLocation - Owner->GetActorLocation()).Size() < DirectAttackRadius
    );
    if (DirectAttack) {
        float Duration = FMath::RandRange(2.0f, 5.0f);
        return FActionTickResult::UnfinishedAnd(new CAttackStanceAction(Target, Duration, false));
    }

    TArray<AInteractiveAgent*> Alerters = Owner->AgentGroupAlerters();
    bool UseAlerter = (
        Alerters->Num() > 0 && FMath::RandRange(0.0f, 1.0f) < Owner->AlertTowerUseChance
    );
    if (UseAlerter) {
        AInteractiveAgent* Alerter = Alerters[FMath::RandRange(0, Alerters.Num() - 1)];
        return FActionTickResult::UnfinishedAnd(new CInteractAction(Alerter));
    }

    AAINavNode* PossibleCover = Manager->UnitAIGetBestCoverNavNodeFor(Owner, Target, 20, 3000.0f);

    bool Assault = (
        PossibleCover == nullptr ||
        Owner->AgentGroupsMembersCount() < 3 ||
        FMath::RandRange(0.0f, 1.0f) < 0.5f
    );
    if (Assault) {
        return FActionTickResult::UnfinishedAnd(new CMoveToPointAction(TargetLocation, DirectAttackRadius, false));
    }

    Manager->UnitAIClaimNavNode(Cover, Owner);

    TArray<CAction*> Parts;
    if ((Cover->GetActorLocation() - TargetLocation).Size() > Cover->ReachDistance) {
        Parts.Push(CMoveToPointAction::MoveToNode(Cover));
    }
    Parts.Push(new CAttackStanceAction(Target, FMath::RandRange(2.0f, 5.0f), Cover->CrouchFullCover));
    return FActionTickResult::UnfinishedAnd(new CMultiAction(Parts));
}

AAgentActor* CAttackAction::ActionCurrentTarget(AUnitAgent* Owner) {
    if (!Owner->AgentHasTargets()) return;

    return Owner->AgentTargets()[0];
}

CAction* CAttackAction::ActionHandleWeapon(AUnitAgent* Owner) {
    AAgentManager* Manager = AAgentManager::AgentsGetManager(Owner->GetWorld());

    AWeaponItem* CurrentWeapon = Owner->UnitActiveWeapon();
    if (CurrentWeapon == nullptr) {
        AItemActor* EquippableWeapon = Owner->UnitInventory()->InventoryGetItemByClass(AWeaponItem::StaticClass());
        if (EquippableWeapon != nullptr) return new CEquipItemAction(EquippableWeapon);
        
        CFindItemAction* FindAttempt = CFindItemAction::FindInWorldOrFail(
            Owner, AWeaponItem::StaticClass(), Owner->DefaultWorldSearchRadius, true
        );
        if (FindAttempt != nullptr) return FindAttempt;

        AAINavNode* HidingSpot = Manager->AIGetNavBestCoverNodeFor(Owner, Target, 30, 4000.0f);
        Manager->AIClaimNavNode(HidingSpot, Owner);
        if (HidingSpot == nullptr) return nullptr; // TODO: Cower.

        float DistanceToHiding = (Owner->GetActorLocation() - HidingSpot->GetActorLocation()).Size();
        if (DistanceToHiding < HidingSpot->ReachDistance * 1.25f) {
            return new CWaitAction(FMath::RandRange(3.0f, 10.0f), false);
        }

        return CMoveToPointAction::MoveToNode(HidingSpot);
    }
    else if (CurrentWeapon->WeaponEmpty()) {
        return new CUseItemAction(CurrentWeapon, nullptr);
    }

    return nullptr;
}

void CAttackAction::ActionHandleTriggerAndFacing(AUnitAgent* Owner) {
    AAgentManager* Manager = AAgentManager::AgentsGetManager(Owner->GetWorld());

    FVector TargetLocation = Target->GetActorLocation();

    float Distance = (Owner->GetActorLocation() - TargetLocation).Size();
    AWeaponItem* Weapon = Owner->UnitActiveWeapon();
    if (Weapon != nullptr && Distance < Weapon->AIFireDistance) {
        State->StateWrite(SK_ATTACK_ACTIVE, 1);

        Owner->UnitFaceTowards(TargetLocation);
        Owner->UnitTorsoPitchTowards(TargetLocation);

        FVector WeaponMuzzle = Owner->UnitActiveWeapon()->WeaponGetMuzzlePosition();

        FHitResult FireCheckHit;
        Owner->GetWorld()->LineTraceSingleByChannel(
            FireCheckHit,
            WeaponMuzzle,
            WeaponMuzzle + Owner->GetActorRotation().RotateVector(FVector(Weapon->AIFireDistance, 0.0f, 0.0f)),
            ECollisionChannel::ECC_Visibility,
            FCollisionQueryParams()
        );

        AAgentActor* FireVictim = Cast<AAgentActor>(FireCheckHit.GetActor());
        bool Fire = (
            FireVictim == nullptr ||
            Manager->AgentsFactionsAreEnemy(FireVictim, Owner, false)
        );
        Owner->UnitSetTriggerPulled(Fire);
    }
    else {
        State->StateWrite(SK_ATTACK_ACTIVE, 0);

        Owner->UnitSetTriggerPulled(false);
        Owner->UnitSetTorsoPitch(0.0f);
    }
}
