// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Items/ItemActor.h"
#include "UnitSlotColliderComponent.h"

#include "UnitSlotComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRANSIENT_API UUnitSlotComponent : public UStaticMeshComponent {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="Inventory Slot")
	AItemActor* Content;
	UPROPERTY(EditAnywhere, Category="Inventory Slot")
	float InventoryViewColliderRadius = 2.0f;
	UPROPERTY(EditAnywhere, Category="Inventory Slot")
	bool UseEquippedItemTransform;

	UUnitSlotColliderComponent* InventoryLookCollider;

public:
	UPROPERTY(EditAnywhere, Category="Inventory Slot")
	TArray<EItemInventoryType> AllowedItems;

public:	
	UUnitSlotComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
	
public:
	AItemActor* SlotGetContent() const;
	void SlotSetContent(AItemActor* NewContent);
};
