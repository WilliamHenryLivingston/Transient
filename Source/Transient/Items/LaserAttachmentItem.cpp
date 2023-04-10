// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "LaserAttachmentItem.h"

#include "../UnitSlotComponent.h"
#include "../UnitPawn.h"
#include "WeaponItem.h"

ALaserAttachmentItem::ALaserAttachmentItem() {
    PrimaryActorTick.bCanEverTick = true;
}

void ALaserAttachmentItem::BeginPlay() {
    Super::BeginPlay();

    this->LaserFX = this->FindComponentByClass<UNiagaraComponent>();
}

void ALaserAttachmentItem::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    bool Active = false;
    AUnitPawn* HolderUnit = nullptr;
    if (this->CurrentHolder != nullptr) {
        HolderUnit = Cast<AUnitPawn>(this->CurrentHolder);
        AWeaponItem* Weapon = HolderUnit->UnitGetActiveWeapon();
        if (Weapon != nullptr) {
            TArray<UUnitSlotComponent*> Slots;
            Weapon->GetComponents(Slots, true);

            for (int i = 0; i < Slots.Num(); i++) {
                if (Slots[i]->SlotGetContent() == this) {
                    Active = true;
                    break;
                }
            }
        }
    }

    this->LaserFX->SetVisibility(Active);
    if (Active) {
        FVector FXBase = this->LaserFX->GetComponentLocation();
        FRotator OuterRotation = HolderUnit->ItemHolderGetRotation();

        FVector LaserMax = FXBase + OuterRotation.RotateVector(FVector(3000.0f, 0.0f, 0.0f));

        FCollisionQueryParams Params;
        Params.AddIgnoredActor(HolderUnit);

        FHitResult HitResult;
        bool Hit = this->GetWorld()->LineTraceSingleByChannel(
            HitResult,
            FXBase, LaserMax,
            ECollisionChannel::ECC_Visibility,
            Params
        );

        FVector LaserEnd = Hit ? HitResult.ImpactPoint : LaserMax;

        this->LaserFX->SetVectorParameter(FName("Beam End"), (OuterRotation + HolderUnit->UnitGetActiveWeapon()->LaserCorrection).UnrotateVector(LaserEnd - FXBase));
    }
}
