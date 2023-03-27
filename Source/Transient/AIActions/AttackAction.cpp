#include "AttackAction.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "../TransientDebug.h"
#include "../AIUnit.h"
#include "../AINavManager.h"
#include "EngageAction.h"
#include "MoveToPointAction.h"

CAttackAction::CAttackAction(AActor* InitTarget) {
    this->Target = InitTarget;
    this->Cover = nullptr;
    this->AttackTime = 0.0f;
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

FAIParentActionTickResult CAttackAction::AIParentActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    if (!IsValid(this->Target)) {
        Owner->UnitSetTriggerPulled(false);
        Owner->UnitSetCrouched(false);

        return FAIParentActionTickResult(true, nullptr);
    }

    this->HandleWeapon(RawOwner);
    
    if (Owner->UnitGetActiveWeapon() == nullptr) {
        // TODO: Flee?
        Owner->UnitSetTriggerPulled(false);
        Owner->UnitSetCrouched(false);

        return FAIParentActionTickResult(true, nullptr);
    }

    FVector TargetLocation = this->Target->GetActorLocation();
    FVector OwnerHeadLocation = Owner->DetectionSourceComponent->GetComponentLocation(); // TODO.
    float Distance = (OwnerHeadLocation - TargetLocation).Size();

    if ((this->Engaging || Distance < 1000.0f)) {
        Owner->UnitFaceTowards(TargetLocation);
        Owner->UnitUpdateTorsoPitch(UKismetMathLibrary::FindLookAtRotation(OwnerHeadLocation, TargetLocation).Pitch);

        FVector WeaponMuzzle = Owner->GetActorLocation() + Owner->UnitGetActiveWeapon()->WeaponGetRelativeMuzzleAsEquipped();

        FHitResult FireCheckHit;
        Owner->GetWorld()->LineTraceSingleByChannel(
            FireCheckHit,
            WeaponMuzzle,
            WeaponMuzzle + Owner->GetActorRotation().RotateVector(FVector(1000.0f, 0.0f, 0.0f)),
            ECollisionChannel::ECC_Visibility,
            FCollisionQueryParams()
        );

        AActor* FireVictim = FireCheckHit.GetActor();
        bool ShouldFire = (
            (
                // Friendly fire check.
                FireVictim == nullptr ||
                !FireVictim->IsA(AUnitPawn::StaticClass()) ||
                Cast<AUnitPawn>(FireVictim)->FactionID != Owner->FactionID
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

    if (!IsValid(this->Target)) {
        Owner->UnitSetTriggerPulled(false);
        Owner->UnitSetCrouched(false);

        return this->Finished;
    }

    this->HandleWeapon(RawOwner);

    AWeaponItem* CurrentWeapon = Owner->UnitGetActiveWeapon();
    if (CurrentWeapon == nullptr) {
        // TODO: Flee? nah, precheck in attack.
        Owner->UnitSetTriggerPulled(false);
        Owner->UnitSetCrouched(false);

        return this->Finished;
    }

    AAINavManager* NavManager = Cast<AAINavManager>(UGameplayStatics::GetActorOfClass(Owner->GetWorld(), AAINavManager::StaticClass()));

    bool Assaulting = Owner->AssaultIfAlone;
    if (Owner->Group != nullptr) {
        Assaulting = Owner->Group->Members.Find(Owner) < (Owner->Group->Members.Num() / 2);
    }
    if (Assaulting && (Owner->GetActorLocation() - this->Target->GetActorLocation()).Size() > CurrentWeapon->AIEngageDistance) {
        this->Engaging = true;
        this->Cover = nullptr;
        NavManager->NavUnclaimAllNodes(Owner);
        return FAIActionTickResult(false, new CMoveToPointAction(this->Target, CurrentWeapon->AIEngageDistance / 2.0f));
    }
    else if (!Assaulting && (this->Cover == nullptr || FMath::RandRange(0.0f, 1.0f) < 0.5f)) {
        this->Engaging = false;
        NavManager->NavUnclaimAllNodes(Owner);

        // ... find cover
        TArray<AAINavNode*> CheckSet = NavManager->NavGetNearestNodes(Owner, 10);

        FVector ThreatLocation = this->Target->GetActorLocation();

        this->Cover = nullptr;
        float CoverScore = 0.0f;
        for (int i = 0; i < CheckSet.Num(); i++) {
            AAINavNode* Check = CheckSet[i];

            if (!Check->CoverPosition) continue;
            if (NavManager->NavIsNodeClaimed(Check)) continue;

            FVector CheckLocation = Check->GetActorLocation();
            float RootDist = (CheckLocation - ThreatLocation).Size();
            float FrontDist = (CheckLocation + (Check->GetActorForwardVector() * 100.0f) - ThreatLocation).Size();

            if (FrontDist >= RootDist) continue;

            float CheckScore = FrontDist - RootDist;

            if (this->Cover == nullptr || CheckScore < CoverScore) {
                this->Cover = Check;
                CoverScore = CheckScore;
            }
        }

        // TODO: Why is this getting hit?
        if (this->Cover == nullptr) return this->Finished; // Broken.

        NavManager->NavClaimNode(Cover, Owner);
        return FAIActionTickResult(false, new CMoveToPointAction(this->Cover, 50.0f));
    }
    else {
        this->Engaging = true;
        return FAIActionTickResult(false, new CEngageAction(this->Target, FMath::RandRange(2.0f, 5.0f), this->Cover != nullptr && this->Cover->CrouchFullCover));
    }
}