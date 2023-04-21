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
    UPROPERTY(EditDefaultsOnly, Category="Inventory")
    float ItemDropRadius = 150.0f;

	// Isomorphic, non-replicated.
    TArray<UInventorySlotComponent*> Slots;

protected:
    virtual void BeginPlay() override;
    virtual void OnComponentDestroyed(bool FullHierarchy) override;

private:
    // Isomorphic.
    float InventoryItemDropRadius();

    UInventorySlotComponent* InventoryFindSlot(bool (*CheckFn)(UInventoryComponent*, AItemActor*));
    UInventorySlotComponent* InventoryFindBestSlotByScore(int MinScore, int (*ScoreFn)(UInventoryComponent*, AItemActor*));
    TArray<UInventorySlotComponent*> InventoryFindSlots(bool (*CheckFn)(UInventoryComponent*, AItemActor*));

public:
    TArray<UInventorySlotComponent*> InventorySlots();

    // TODO: Problem with these is external stuff needs to indirectly interact with the inventory (wants items not slots).
	TArray<UInventorySlotComponent*> InventoryFindOrderedEquippableSlots();
	TArray<UInventorySlotComponent*> InventoryFindSlotsByItemTag(FString Tag);
	TArray<UInventorySlotComponent*> InventoryFindSlotsByItemClass(TSubclassOf<AItemActor> ItemClass);

	UInventorySlotComponent* InventoryFindSlotByName(FString SlotName);
	UInventorySlotComponent* InventoryFindSlotWithItem(AItemActor* Target);

	UInventorySlotComponent* InventoryFindBestEmptySlotAllowing(EItemInventoryType Type);
	UInventorySlotComponent* InventoryFindBestSlotAllowing(EItemInventoryType Type);

    AItemActor* InventoryFindBestItemOfClass(TSubclassOf<AItemActor> ItemClass);
};
