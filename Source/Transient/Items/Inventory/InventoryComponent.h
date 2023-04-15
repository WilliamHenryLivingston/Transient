// Copyright: R. Saxifrage, 2023. All rights reserved.

// Inventory components manage the set of inventory slots within their parent actor.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Items/ItemActor.h"

#include "InventorySlotComponent.h"

#include "InventoryComponent.generated.h"

UCLASS(ClassGroup=Transient, meta=BlueprintSpawnableComponent)
class UInventoryComponent : public USceneComponent {
    GENERATED_BODY()

private:
	// Isomorphic, non-replicated.
    TArray<UInventorySlotComponent*> Slots;

protected:
    virtual void BeginPlay() override;

public:
    // Isomorphic.
    TArray<UInventorySlotComponent*> InventorySlots();
	TArray<UInventorySlotComponent*> InventoryEquippableSlots();

	TArray<UInventorySlotComponent*> InventoryFindSlotsByItemTag(FString Tag);
	TArray<UInventorySlotComponent*> InventoryFindSlotsByItemClass(TSubclassOf<AItemActor> ItemClass);

	UInventorySlotComponent* InventoryFindSlotWithItem(AItemActor* Target);

	UUnitSlotComponent* InventoryFindBestEmptySlotAllowing(EItemInventoryType Type);
	UUnitSlotComponent* InventoryFindBestSlotAllowing(EItemInventoryType Type);
};
