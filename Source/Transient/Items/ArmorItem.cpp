// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "ArmorItem.h"

#include "../UnitSlotComponent.h"

void AArmorItem::BeginPlay() {
    Super::BeginPlay();

    this->DefaultVariant = this->VisibleComponent->GetStaticMesh();
}

void AArmorItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AArmorItem, this->KineticHealth);
}

void AArmorItem::ArmorHealthChanged() {
    if (this->DamagedVariant != nullptr && this->KineticHealth < this->MaxKineticHealth / 2.0f) {
        this->VisibleComponent->SetStaticMesh(this->DamagedVariant);
    }
    else {
        this->VisibleComponent->SetStaticMesh(this->DefaultVariant);
    }
}

// TODO: Default damagable behavior?
void AArmorItem::DamagableTakeDamage_Implementation(FDamageProfile Profile, AActor* Cause, AActor* Source) {
    this->KineticHealth -= Profile.Kinetic;

    if (this->KineticHealth <= 0.0f) this->Destroy();
}
