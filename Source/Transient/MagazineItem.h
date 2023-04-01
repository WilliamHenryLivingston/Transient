// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ItemActor.h"
#include "ProjectileActor.h"

#include "MagazineItem.generated.h"

UCLASS()
class TRANSIENT_API AMagazineItem : public AItemActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Magazine")
	TSubclassOf<AProjectileActor> ProjectileType;
	UPROPERTY(EditAnywhere, Category="Magazine")
	int Ammo;
	UPROPERTY(EditAnywhere, Category="Magazine")
	int Capacity;
	UPROPERTY(EditAnywhere, Category="Magazine")
	int AmmoTypeID;

	UPROPERTY(EditDefaultsOnly, Category="Magazine")
	TArray<UStaticMesh*> StateVariants;

public:
	AMagazineItem();
	virtual void Tick(float DeltaTime) override;
};
