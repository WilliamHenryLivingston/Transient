// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "InventoryComponent.h"

void UInventoryComponent::BeginPlay() {
    this->GetOwner()->GetComponents(this->InventorySlots, false);
}

void UInventoryComponent::OnComponentDestroyed(bool FullHierarchy) {
    Super::OnComponentDestroyed(FullHierarchy);

    if (!this->GetOwner()->HasAuthority()) return;

    for (int i = 0; i < this->Slots.Num(); i++) {
        this->Slots[i]->SlotSetContent(nullptr);
    }
}

float UInventoryComponent::InventoryItemDropRadius() { return this->ItemDropRadius; }

TArray<UInventorySlotComponent*> UInventoryComponent::InventorySlots() { return this->InventorySlots; }

UInventorySlotComponent* UInventoryComponent::InventoryFindSlot(
    bool (*CheckFn)(UInventorySlotComponent*, AItemActor*)
) {
	for (int i = 0; i < this->Slots.Num(); i++) {
		UInventorySlotComponent* Check = this->Slots[i];

		if (CheckFn(Check, Check->SlotContent())) return Check;
	}

    return nullptr;
}

UInventorySlotComponent* UInventoryComponent::InventoryFindBestSlotByScore(
    int MinScore, int (*CheckFn)(UInventorySlotComponent*, AItemActor*)
) {
    int BestScore = MinScore;
    UInventorySlotComponent* Best = nullptr;

	for (int i = 0; i < this->Slots.Num(); i++) {
		UInventorySlotComponent* Check = this->Slots[i];

        int CheckScore = CheckFn(Check, Check->SlotContent());
		if (CheckScore > BestScore) {
            Best = Check;
            BestScore = CheckScore;
        }
	}

    return Best;
}

TArray<UInventorySlotComponent*> UInventoryComponent::InventoryFindSlots(
    bool (*CheckFn)(UInventorySlotComponent*, AItemActor*)
) {
    TArray<UInventorySlotComponent*> Found;

	for (int i = 0; i < this->Slots.Num(); i++) {
		UInventorySlotComponent* Check = this->Slots[i];

		if (CheckFn(Check, Check->SlotContent())) Found.Push(Check);
	}

    return Found;
}

TArray<UInventorySlotComponent*> UInventoryComponent::InventoryFindOrderedEquippableSlots() {
    TArray<UInventorySlotComponent*> Found = this->InventoryFindSlots(
        [](UInventorySlotComponent* Slot, AItemActor* Content) {
            return Content != nullptr && Content->Equippable;
        }
    );

	Found.Sort(
        [](const UInventorySlotComponent& A, const UInventorySlotComponent& B) {
		    return A.SlotContent()->InventoryType < B.SlotContent()->InventoryType; // TODO: Better ordering.
	    }
    );

	return Found;
}

TArray<UInventorySlotComponent*> InventoryFindSlotsByItemTag(FString Tag) {
    return this->InventoryFindSlots(
        [](UInventorySlotComponent* Slot, AItemActor* Content) {
            return Content != nullptr && Content->Tags.Contains(Tag);
        }
    );
}

TArray<UInventorySlotComponent*> InventoryFindSlotsByItemClass(TSubclassOf<AItemActor> ItemClass) {
    return this->InventoryFindSlots(
        [](UInventorySlotComponent* Slot, AItemActor* Content) {
            return Content != nullptr && Content->IsA(ItemClass);
        }
    );
}

UInventorySlotComponent* UInventoryComponent::InventoryFindSlotWithItem(AItemActor* Target) {
    return this->InventoryFindSlot(
        [](UInventorySlotComponent* Slot, AItemActor* Content) {
            return Content == Target;
        }
    );
}

UInventorySlotComponent* UInventoryComponent::InventoryFindSlotByName(FString Name) {
    return this->InventoryFindSlot(
        [](UInventorySlotComponent* Slot, AItemActor* Content) {
            return Slot->SlotUniqueName().Equals(Name);
        }
    );
}

UInventorySlotComponent* UInventoryComponent::InventoryFindBestEmptySlotAllowing(EItemInventoryType Type) {
    return this->InventoryFindBestSlotByScore(
        0, [](UInventorySlotComponent* Slot, AItemActor* Content) {
            if (Content != nullptr) return -1;

            TArray<EItemInventoryType> AllowedTypes = Slot->SlotAllowedTypes();
            if (!AllowedTypes.Contains(Type)) return -1;

            return 30 - AllowedTypes.Num();
        }
    );
}

UInventorySlotComponent* UInventoryComponent::InventoryFindBestSlotAllowing(EItemInventoryType Type) {
    return this->InventoryFindBestSlotByScore(
        0, [](UInventorySlotComponent* Slot, AItemActor* Content) {
            TArray<EItemInventoryType> AllowedTypes = Slot->SlotAllowedTypes();
            if (!AllowedTypes.Contains(Type)) return -1;

            return 30 - AllowedTypes.Num();
        }
    );
}
