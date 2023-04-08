#include "UseItemAction.h"

#include "Kismet/KismetMathLibrary.h"

#include "../AIUnit.h"
#include "EquipItemAction.h"

CUseItemAction::CUseItemAction(AItemActor* InitTarget, AActor* InitUseTarget) {
    this->Target = InitTarget;
    this->UseTarget = InitUseTarget;
    this->Timer = InitUseTarget != nullptr ? 1.0f : -1.0f;
    this->UseStarted = false;
}

CUseItemAction::~CUseItemAction() {}

FAIActionTickResult CUseItemAction::AIActionTick(AActor* RawOwner, float DeltaTime) {
    AAIUnit* Owner = Cast<AAIUnit>(RawOwner);

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

        Owner->UnitUseActiveItem(this->UseTarget);
        this->UseStarted = true;
        return this->Unfinished;
    }

    Owner->UnitUpdateTorsoPitch(0.0f);
    return this->Finished;
}