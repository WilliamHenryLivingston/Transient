#pragma once

#include "CoreMinimal.h"

#include "ItemActor.h"

#include "ArmorItem.generated.h"

UCLASS()
class TRANSIENT_API AArmorItem : public AItemActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UStaticMesh* EquippedMesh;

	UPROPERTY(EditAnywhere)
	float Health;
};
