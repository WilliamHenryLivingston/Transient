#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ItemActor.h"

#include "UnitSlotComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRANSIENT_API UUnitSlotComponent : public UStaticMeshComponent {
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="Inventory Slot")
	AItemActor* Content;

public:
	UPROPERTY(EditAnywhere, Category="Inventory Slot")
	TArray<EItemInventoryType> AllowedItems;

public:	
	UUnitSlotComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
	
public:
	AItemActor* SlotGetContent();
	void SlotSetContent(AItemActor* NewContent);
};
