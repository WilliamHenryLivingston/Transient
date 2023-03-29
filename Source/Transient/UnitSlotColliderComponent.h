#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"

#include "UnitSlotColliderComponent.generated.h"

UCLASS()
class TRANSIENT_API UUnitSlotColliderComponent : public USphereComponent {
	GENERATED_BODY()

public:
	USceneComponent* ParentSlot;
};
