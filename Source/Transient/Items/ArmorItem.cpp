// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "ArmorItem.h"

#include "../UnitSlotComponent.h"

void AArmorItem::BeginPlay() {
    Super::BeginPlay();

    this->DefaultVariant = this->VisibleComponent->GetStaticMesh();
}

void AArmorItem::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if (this->DamagedVariant != nullptr && this->KineticHealth < this->MaxKineticHealth / 2.0f) {
        this->VisibleComponent->SetStaticMesh(this->DamagedVariant);
    }
    else {
        this->VisibleComponent->SetStaticMesh(this->DefaultVariant);
    }
}

void AArmorItem::DamagableTakeDamage(FDamageProfile Profile, AActor* Cause, AActor* Source) {
    this->KineticHealth -= Profile.Kinetic;
    if (this->KineticHealth <= 0.0f) {

        // TODO: Duplicate from unit.
        TArray<UUnitSlotComponent*> Slots;
        this->GetComponents(Slots, false);
        for (int i = 0; i < Slots.Num(); i++) {
            UUnitSlotComponent* Slot = Slots[i];

            AItemActor* Content = Slot->SlotGetContent();
            if (Content == nullptr) continue;

            Slot->SlotSetContent(nullptr);
            Content->ItemDrop(this);
        }

        this->Destroy();
    }
}