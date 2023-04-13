#include "AgroAction.h"

#include "Kismet/KismetMathLibrary.h"

#include "../../Items/WeaponItem.h"
#include "../../Environment/InteractiveActor.h"
#include "../../Debug.h"
#include "../AIUnit.h"
#include "../AIManager.h"
#include "EngageAction.h"
#include "MoveToPointAction.h"
#include "FindItemAction.h"
#include "InteractAction.h"
#include "EquipItemAction.h"
#include "UseItemAction.h"
#include "MultiAction.h"
#include "AIState.h"

CAgroAction::CAgroAction(AActor* InitTarget) {
    this->Target = InitTarget;

    this->DebugInfo = FString::Printf(TEXT("agro %s"), *this->Target->GetName());
}

CAgroAction::~CAgroAction() {}

AActor* CAgroAction::AIActionAgroTarget() {
    return this->Target;
}

IAIActionExecutor* CAgroAction::HandleWeapon(AActor* RawOwner) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);
    AAIManager* Manager = AAIManager::AIGetManagerInstance(Owner->GetWorld());

    AWeaponItem* CurrentWeapon = Owner->UnitGetActiveWeapon();
    if (CurrentWeapon == nullptr) {
        // Equip if in inventory.
        AItemActor* EquippableWeapon = Owner->UnitGetItemByClass(AWeaponItem::StaticClass());
        if (EquippableWeapon != nullptr) {
            return new CEquipItemAction(EquippableWeapon);
        }
        
        // Try to find in world.
        CFindItemAction* FindAttempt = new CFindItemAction(RawOwner, AWeaponItem::StaticClass(), Owner->DefaultWorldSearchRadius, true);
        if (FindAttempt->Target != nullptr) return FindAttempt;

        delete FindAttempt;
        
        // Flee.
        AAINavNode* HidingSpot = Manager->AIGetNavBestCoverNodeFor(Owner, this->Target, 30, 4000.0f);
        Manager->AIClaimNavNode(HidingSpot, Owner);
        if (HidingSpot == nullptr) return nullptr; // TODO: Action totally busted.

        return new CMoveToPointAction(HidingSpot, HidingSpot->ReachDistance);
    }
    if (CurrentWeapon != nullptr && CurrentWeapon->WeaponEmpty()) {
        return new CUseItemAction(CurrentWeapon, nullptr);
    }

    return nullptr;
}

bool CAgroAction::HandleTriggerAndFacing(AActor* RawOwner) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);
    AAIManager* Manager = AAIManager::AIGetManagerInstance(Owner->GetWorld());

    bool IsFiring = false;

    FVector TargetLocation = this->Target->GetActorLocation();
    FVector OwnerHeadLocation = Owner->DetectionSourceComponent->GetComponentLocation();

    float Distance = (OwnerHeadLocation - TargetLocation).Size();
    AWeaponItem* Weapon = Owner->UnitGetActiveWeapon();
    if (Weapon != nullptr && Distance < Weapon->AIFireDistance) {
        Owner->AIState.Emplace(STATE_AGRO_CONTROL, 1);

        Owner->UnitFaceTowards(TargetLocation);
        Owner->UnitUpdateTorsoPitch(UKismetMathLibrary::FindLookAtRotation(OwnerHeadLocation, TargetLocation).Pitch);

        FVector WeaponMuzzle = Owner->UnitGetActiveWeapon()->WeaponGetMuzzlePosition();

        FHitResult FireCheckHit;
        Owner->GetWorld()->LineTraceSingleByChannel(
            FireCheckHit,
            WeaponMuzzle,
            WeaponMuzzle + Owner->GetActorRotation().RotateVector(FVector(Weapon->AIFireDistance, 0.0f, 0.0f)),
            ECollisionChannel::ECC_Visibility,
            FCollisionQueryParams()
        );

        AActor* FireVictim = FireCheckHit.GetActor();
        IsFiring = (
            (
                // Friendly fire check.
                FireVictim == nullptr ||
                !FireVictim->IsA(AUnitPawn::StaticClass()) ||
                Manager->AIIsFactionEnemy(Cast<AUnitPawn>(FireVictim)->FactionID, Owner->FactionID, false)
            )
        );
        Owner->UnitSetTriggerPulled(IsFiring);
    }
    else {
        Owner->AIState.Emplace(STATE_AGRO_CONTROL, 0);

        Owner->UnitSetTriggerPulled(false);
        Owner->UnitUpdateTorsoPitch(0.0f);
    }

    return IsFiring;
}

FAIParentActionTickResult CAgroAction::AIParentActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    if (!IsValid(this->Target)) {
        Owner->UnitSetTriggerPulled(false);
        Owner->UnitSetCrouched(false);

        return FAIParentActionTickResult(true, nullptr);
    }

    this->HandleTriggerAndFacing(RawOwner);

    bool IsHealing = (
        Owner->AIState.FindOrAdd(STATE_E_HEAL, 0) == 0 ||
        Owner->AIState.FindOrAdd(STATE_K_HEAL, 0) == 0
    );
    if (IsHealing) {
        return FAIParentActionTickResult(false, nullptr);
    }

    IAIActionExecutor* WeaponTask = this->HandleWeapon(RawOwner);
    if (WeaponTask != nullptr) {
        return FAIParentActionTickResult(false, WeaponTask);
    }

    return FAIParentActionTickResult(false, nullptr);
}

FAIActionTickResult CAgroAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);
    AAIManager* Manager = AAIManager::AIGetManagerInstance(Owner->GetWorld());

    Manager->AIUnclaimAllNavNodes(Owner);

    if (!IsValid(this->Target)) {
        Owner->UnitSetTriggerPulled(false);
        Owner->UnitSetCrouched(false);

        return this->Finished;
    }

    IAIActionExecutor* WeaponTask = this->HandleWeapon(RawOwner);
    if (WeaponTask != nullptr) {
        return FAIActionTickResult(false, WeaponTask);
    }

    AWeaponItem* Weapon = Owner->UnitGetActiveWeapon();
    if (Weapon == nullptr) {
        ERR_LOG(Owner, "no possible agro actions");
        return this->Unfinished;
    }

    this->HandleTriggerAndFacing(RawOwner);

    FVector TargetLocation = this->Target->GetActorLocation();
    float DirectAttackRadius = Weapon->AIFireDistance / 2.0f;
    float ThreatDistance = (TargetLocation - Owner->GetActorLocation()).Size();

    if (ThreatDistance < DirectAttackRadius) {
        return FAIActionTickResult(false, new CEngageAction(this->Target, FMath::RandRange(2.0f, 5.0f), false));
    }
    else if (Owner->Group != nullptr) {
        TArray<AActor*>* AlertTowers = &Owner->Group->AlertTowers;
        if (AlertTowers->Num() > 0 && FMath::RandRange(0.0f, 1.0f) < Owner->AlertTowerUseChance) {
            AInteractiveActor* Tower = Cast<AInteractiveActor>((*AlertTowers)[FMath::RandRange(0, AlertTowers->Num() - 1)]);
            
            if (Tower != nullptr) return FAIActionTickResult(false, new CInteractAction(Tower));
        }
    }

    bool ShouldAssault = (
        Owner->Group == nullptr || Owner->Group->Members.Num() < 3 ||
        FMath::RandRange(0.0f, 1.0f) < 0.5f
    );

    if (!ShouldAssault) {
        AAINavNode* Cover = Manager->AIGetNavBestCoverNodeFor(Owner, this->Target, 20, 3000.0f);

        if (Cover != nullptr) {
            Manager->AIClaimNavNode(Cover, Owner);

            TArray<IAIActionExecutor*> Parts;
            if ((Cover->GetActorLocation() - TargetLocation).Size() > Cover->ReachDistance) {
                Parts.Push(new CMoveToPointAction(Cover, Cover->ReachDistance));
            }
            Parts.Push(new CEngageAction(this->Target, FMath::RandRange(2.0f, 5.0f), Cover->CrouchFullCover));

            return FAIActionTickResult(false, new CMultiAction(Parts));
        }
    }

    return FAIActionTickResult(false, new CMoveToPointAction(this->Target, DirectAttackRadius));
}
