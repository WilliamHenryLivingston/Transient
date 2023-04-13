#include "UseItemAction.h"

#include "Kismet/KismetMathLibrary.h"

#include "../../Items/WeaponItem.h"
#include "../AIUnit.h"
#include "EquipItemAction.h"

CUseItemAction::CUseItemAction(AItemActor* InitTarget, AActor* InitUseTarget) {
    this->Target = InitTarget;
    this->UseTarget = InitUseTarget;
    this->Timer = InitUseTarget != nullptr ? 1.0f : -1.0f;
    this->UseStarted = false;

    FString Name = TEXT("<current>");
    if (this->Target != nullptr) {
        Name = this->Target->GetName();
    }
    this->DebugInfo = FString::Printf(TEXT("use %s"), *Name);
}

CUseItemAction::~CUseItemAction() {}

FAIActionTickResult CUseItemAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

    if (this->Target == nullptr) this->Target = Owner->UnitGetActiveItem();

    if (Owner->UnitAreArmsOccupied()) return this->Unfinished;

    if (!this->UseStarted) {
        if (Owner->UnitGetActiveItem() != this->Target) {
            return FAIActionTickResult(false, new CEquipItemAction(this->Target));
        }

        if (this->Timer > 0.0f) {
            this->Timer -= DeltaTime;
            
            if (this->UseTarget != nullptr) {
                FVector TargetLocation = this->UseTarget->GetActorLocation();
                FVector OwnerHeadLocation = Owner->DetectionSourceComponent->GetComponentLocation();
                Owner->UnitUpdateTorsoPitch(UKismetMathLibrary::FindLookAtRotation(OwnerHeadLocation, TargetLocation).Pitch);
                Owner->UnitFaceTowards(TargetLocation);
            }
            return this->Unfinished;
        }

        AWeaponItem* AsWeapon = Cast<AWeaponItem>(this->Target);
        if (AsWeapon != nullptr) Owner->UnitReload();
        else Owner->UnitUseActiveItem(this->UseTarget);

        this->UseStarted = true;
        return this->Unfinished;
    }

    Owner->UnitUpdateTorsoPitch(0.0f);
    return this->Finished;
}
