// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "InventoryComponent.h"

void UInventoryComponent::BeginPlay() {
    this->GetOwner()->GetComponents(this->InventorySlots, false);
}

TArray<UInventorySlotComponent*> InventorySlots() { return this->InventorySlots; }
