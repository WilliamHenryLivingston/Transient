// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "ArmorItem.h"

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
