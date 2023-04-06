// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

#include "../Animation/UnitAnimInstance.h"
#include "ItemHolder.h"

#include "ItemActor.generated.h"

UENUM(BlueprintType)
enum class EItemInventoryType : uint8 {
    NA,
	Weapon,
	WeaponLarge,
	Container,
	Pouch,
	Arrow,
	Magazine,
	Misc,
	MiscLarge
};

UCLASS()
class TRANSIENT_API AItemActor : public AActor {
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category="Item Inventory Config")
	FRotator EquippedRotation;
	UPROPERTY(EditDefaultsOnly, Category="Item Inventory Config")
	FVector EquippedOffset;
	UPROPERTY(EditDefaultsOnly, Category="Item Inventory Config")
	FRotator InSlotRotation;
	UPROPERTY(EditDefaultsOnly, Category="Item Inventory Config")
	FVector InSlotOffset;
	UPROPERTY(EditDefaultsOnly, Category="Item Inventory Config")
	float SlotColliderModifier = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category="Item Inventory Config")
	EItemInventoryType InventoryType;
	UPROPERTY(EditDefaultsOnly, Category="Item Inventory Config")
	bool Equippable;
	UPROPERTY(EditDefaultsOnly, Category="Item Inventory Config")
	float EquippedTorsoYaw;
	UPROPERTY(EditDefaultsOnly, Category="Item Inventory Config")
	bool EquipAltHand;
	UPROPERTY(EditAnywhere, Category="Item Inventory Config")
	EUnitAnimArmsState EquippedAnimArmsMode;

	UPROPERTY(EditAnywhere, Category="Item Inventory Config")
	FString ItemName;

	UPROPERTY(EditDefaultsOnly, Category="Item")
	UStaticMeshComponent* VisibleComponent;

protected:
	IItemHolder* CurrentHolder;

private:
	UPROPERTY(EditDefaultsOnly, Category="Item")
	UBoxComponent* ColliderComponent;

public:
	AItemActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual FString ItemGetDescriptorString();
	virtual void ItemTake(IItemHolder* Target);
	virtual void ItemDrop(IItemHolder* Target);
};
