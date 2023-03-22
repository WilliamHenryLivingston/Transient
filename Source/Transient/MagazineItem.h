#pragma once

#include "CoreMinimal.h"

#include "ItemActor.h"

#include "MagazineItem.generated.h"

UCLASS()
class TRANSIENT_API AMagazineItem : public AItemActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UStaticMesh* EquippedMesh;

	UPROPERTY(EditAnywhere)
	int Ammo;

	UPROPERTY(EditAnywhere)
	int AmmoTypeID;
};
