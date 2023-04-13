// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "../Damagable.h"
#include "ItemActor.h"

#include "ArmorItem.generated.h"

UCLASS()
class TRANSIENT_API AArmorItem : public AItemActor, public IDamagable {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Armor")
	float KineticHealth = 300.0f;
	UPROPERTY(EditAnywhere, Category="Armor")
	float MaxKineticHealth = 300.0f;
	UPROPERTY(EditAnywhere, Category="Armor")
	bool Heavy;

private:
	UPROPERTY(EditAnywhere, Category="Armor")
	UStaticMesh* DamagedVariant;

	UStaticMesh* DefaultVariant;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	virtual void DamagableTakeDamage(FDamageProfile Profile, AActor* Cause, AActor* Source) override;
};
