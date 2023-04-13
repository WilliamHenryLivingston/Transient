#include "AttackAction.h"

#include "Kismet/KismetMathLibrary.h"

#include "../../Items/WeaponItem.h"
#include "../AIUnit.h"
#include "../AIManager.h"
#include "EngageAction.h"
#include "MoveToPointAction.h"
#include "FindItemAction.h"
#include "AIState.h"

CAttackAction::CAttackAction(AActor* InitTarget) {
    this->Target = InitTarget;
    this->Cover = nullptr;
    this->AttackTime = 0.0f;

    this->DebugInfo = FString::Printf(TEXT("attack %s"), *this->Target->GetName());
}

CAttackAction::~CAttackAction() {}

bool CAttackAction::AIActionIsAttackOn(AActor* Check) {
    return Check == this->Target;
}

void CAttackAction::HandleWeapon(AActor* RawOwner) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    AWeaponItem* CurrentWeapon = Owner->UnitGetActiveWeapon();
    if (CurrentWeapon == nullptr) {
        AItemActor* EquippableWeapon = Owner->UnitGetItemByClass(AWeaponItem::StaticClass());
        Owner->UnitEquipItem(EquippableWeapon);

        CurrentWeapon = Owner->UnitGetActiveWeapon();
    }
    if (CurrentWeapon != nullptr && CurrentWeapon->WeaponEmpty()) {
        Owner->UnitReload();
    }
}

AAINavNode* CAttackAction::FindCover(AActor* RawOwner, AActor* From, int CheckNodes, float MaxDistance) {
    FVector OwnerLocation = RawOwner->GetActorLocation();
    FVector ThreatLocation = From->GetActorLocation();

    AAIManager* Manager = AAIManager::AIGetManagerInstance(RawOwner->GetWorld());

    TArray<AAINavNode*> CheckSet = Manager->AIGetNavNearestNodes(RawOwner, CheckNodes);

    AAINavNode* FoundCover = nullptr;
    float CoverScore = 0.0f;
    for (int i = 0; i < CheckSet.Num(); i++) {
        AAINavNode* Check = CheckSet[i];

        if (!Check->CoverPosition) continue;
        if (Manager->AIIsNavNodeClaimed(Check)) continue;

        FVector CheckLocation = Check->GetActorLocation();
        if (MaxDistance > 0.0f && (CheckLocation - OwnerLocation).Size() > MaxDistance) continue;

        float RootDist = (CheckLocation - ThreatLocation).Size();
        float FrontDist = (CheckLocation + (Check->GetActorForwardVector() * 100.0f) - ThreatLocation).Size();
        if (FrontDist >= RootDist) continue;

        float CheckScore = FrontDist - RootDist;
        if (FoundCover == nullptr || CheckScore < CoverScore) {
            FoundCover = Check;
            CoverScore = CheckScore;
        }
    }

    return FoundCover;
}

IAIActionExecutor* CAttackAction::FleeOrArm(AActor* RawOwner) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    CFindItemAction* FindAttempt = new CFindItemAction(RawOwner, AWeaponItem::StaticClass(), Owner->DefaultWorldSearchRadius, true);
    if (FindAttempt->Target != nullptr) return FindAttempt;

    AAINavNode* FoundCover = this->FindCover(RawOwner, this->Target, 30, -1.0f);
    if (FoundCover == nullptr) return nullptr;

    AAIManager* Manager = AAIManager::AIGetManagerInstance(Owner->GetWorld());

    Manager->AIClaimNavNode(FoundCover, RawOwner);
    return new CMoveToPointAction(FoundCover, FoundCover->ReachDistance);
}

FAIParentActionTickResult CAttackAction::AIParentActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    if (!IsValid(this->Target)) {
        Owner->UnitSetTriggerPulled(false);
        Owner->UnitSetCrouched(false);

        return FAIParentActionTickResult(true, nullptr);
    }

    this->HandleWeapon(RawOwner);

    bool IsHealing = (
        Owner->AIState.FindOrAdd(STATE_E_HEAL, 0) == 0 ||
        Owner->AIState.FindOrAdd(STATE_K_HEAL, 0) == 0
    );
    if (IsHealing) return FAIParentActionTickResult(false, nullptr);

    if (Owner->UnitGetActiveWeapon() == nullptr) {
        Owner->UnitSetTriggerPulled(false);
        Owner->UnitSetCrouched(false);

        IAIActionExecutor* DefensiveAction = this->FleeOrArm(Owner);
        if (DefensiveAction == nullptr) {
            // Broken!
            return FAIParentActionTickResult(true, nullptr);
        }

        return FAIParentActionTickResult(false, DefensiveAction);
    }

    FVector TargetLocation = this->Target->GetActorLocation();
    FVector OwnerHeadLocation = Owner->DetectionSourceComponent->GetComponentLocation(); // TODO.
    float Distance = (OwnerHeadLocation - TargetLocation).Size();

    float EngageLimit = Owner->UnitGetActiveWeapon()->AIFireDistance;
    if (this->Engaging && Distance < EngageLimit) {
        Owner->UnitFaceTowards(TargetLocation);
        Owner->UnitUpdateTorsoPitch(UKismetMathLibrary::FindLookAtRotation(OwnerHeadLocation, TargetLocation).Pitch);

        FVector WeaponMuzzle = Owner->UnitGetActiveWeapon()->WeaponGetMuzzlePosition();

        FHitResult FireCheckHit;
        Owner->GetWorld()->LineTraceSingleByChannel(
            FireCheckHit,
            WeaponMuzzle,
            WeaponMuzzle + Owner->GetActorRotation().RotateVector(FVector(EngageLimit, 0.0f, 0.0f)),
            ECollisionChannel::ECC_Visibility,
            FCollisionQueryParams()
        );

        AAIManager* Manager = AAIManager::AIGetManagerInstance(Owner->GetWorld());

        AActor* FireVictim = FireCheckHit.GetActor();
        bool ShouldFire = (
            (
                // Friendly fire check.
                FireVictim == nullptr ||
                !FireVictim->IsA(AUnitPawn::StaticClass()) ||
                Manager->AIIsFactionEnemy(Cast<AUnitPawn>(FireVictim)->FactionID, Owner->FactionID, false)
            ) && (
                // Shoot at cover check.
                this->Cover == nullptr ||
                !this->Cover->CrouchFullCover ||
                !Owner->UnitIsCrouched()
            )
        );
        Owner->UnitSetTriggerPulled(ShouldFire);
    }
    else {
        Owner->UnitSetTriggerPulled(false);
        Owner->UnitUpdateTorsoPitch(0.0f);
    }

    return FAIParentActionTickResult(false, nullptr);
}

FAIActionTickResult CAttackAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    Owner->AIState.Emplace(STATE_THREATS, Owner->AIState.FindOrAdd(STATE_THREATS, 0) + 1);

    if (!IsValid(this->Target)) {
        Owner->UnitSetTriggerPulled(false);
        Owner->UnitSetCrouched(false);

        Owner->AIState.Emplace(STATE_THREATS, Owner->AIState.FindOrAdd(STATE_THREATS, 1) - 1);
        return this->Finished;
    }

    this->HandleWeapon(RawOwner);

    AWeaponItem* CurrentWeapon = Owner->UnitGetActiveWeapon();
    if (CurrentWeapon == nullptr) {
        Owner->UnitSetTriggerPulled(false);
        Owner->UnitSetCrouched(false);

        Owner->AIState.Emplace(STATE_THREATS, Owner->AIState.FindOrAdd(STATE_THREATS, 1) - 1);
        
        IAIActionExecutor* DefensiveAction = this->FleeOrArm(Owner);
        if (DefensiveAction == nullptr) {
            // Broken!
            return FAIActionTickResult(true, nullptr);
        }

        return FAIActionTickResult(false, DefensiveAction);
    }

    AAIManager* Manager = AAIManager::AIGetManagerInstance(Owner->GetWorld());

    bool Assaulting = Owner->AssaultIfAlone;
    if (Owner->Group != nullptr) {
        Assaulting = Owner->Group->Members.Find(Owner) > (Owner->Group->Members.Num() / 2);
    }
    if (Assaulting && (Owner->GetActorLocation() - this->Target->GetActorLocation()).Size() > CurrentWeapon->AIEngageDistance) {
        this->Engaging = true;
        this->Cover = nullptr;
        Manager->AIUnclaimAllNavNodes(Owner);
        return FAIActionTickResult(false, new CMoveToPointAction(this->Target, CurrentWeapon->AIFireDistance / 2.0f));
    }
    else if (!Assaulting && (this->Cover == nullptr || FMath::RandRange(0.0f, 1.0f) < 0.5f)) {
        this->Engaging = false;
        Manager->AIUnclaimAllNavNodes(Owner);

        this->Cover = this->FindCover(Owner, this->Target, 30, 4000.0f);
        if (this->Cover == nullptr) return this->Finished; // Broken.

        Manager->AIClaimNavNode(this->Cover, Owner);
        return FAIActionTickResult(false, new CMoveToPointAction(this->Cover, 50.0f));
    }
    else {
        this->Engaging = true;
        return FAIActionTickResult(false, new CEngageAction(this->Target, FMath::RandRange(2.0f, 5.0f), this->Cover != nullptr && this->Cover->CrouchFullCover));
    }
}
