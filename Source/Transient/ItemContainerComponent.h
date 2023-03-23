#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "ItemContainerComponent.generated.h"

USTRUCT()
struct FContainerSlot {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Container Slot Config")
	FVector Offset;
	UPROPERTY(EditAnywhere, Category="Container Slot Config")
	FRotator Rotation;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRANSIENT_API UItemContainerComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Item Inventory Config")
	TArray<FContainerSlot> ContainerSlots;

public:
	UItemContainerComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
};
