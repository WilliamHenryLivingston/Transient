// Copyright: R. Saxifrage, 2023. All rights reserved.

// An inventory slot can hold an attached item actor of a given type. The
// replication strategy for item attachment is that items have a replicated
// pointer to their current slot, and handle attach / detach themselves in a
// callback.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Items/ItemActor.h"

#include "InventorySlotComponent.generated.h"

class UInventoryComponent;

UCLASS(ClassGroup=Transient, meta=BlueprintSpawnableComponent)
class UInventorySlotComponent : public USceneComponent {
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category="Inventory Slot")
	bool UseEquippedItemTransform;
	UPROPERTY(EditDefaultsOnly, Category="Inventory Slot")
	TArray<EItemInventoryType> AllowedItems;

	AItemActor* Content; // See above for why non-replicated.

	// Isomorphic, non-replicated.
	UInventoryComponent* ParentInventory;

protected:
	virtual void BeginPlay() override;

public:
	// Isomorphic.
	UInventoryComponent* SlotInventory();
	TArray<EItemInventoryType> SlotAllowedTypes();
	AItemActor* SlotContent() const;

	void SlotSetContentReplicated(AItemActor* Item); // Called by items on replicate.

	// Game logic.
	void SlotSetContent(AItemActor* NewContent);
};
