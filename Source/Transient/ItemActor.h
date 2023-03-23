#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

#include "ItemHolder.h"
#include "UnitAnimInstance.h"
#include "EquippedMeshConfig.h"
#include "ItemContainerComponent.h"

#include "ItemActor.generated.h"

UENUM(BlueprintType)
enum class EItemInventoryType : uint8 {
    NA,
	Weapon,
	WeaponLarge,
	Container,
	Magazine,
	Misc
};

UCLASS()
class TRANSIENT_API AItemActor : public AActor {
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="Item Inventory Config")
	FEquippedMeshConfig EquippedMesh; // TODO: Rename to InventoryMesh.
	UPROPERTY(EditDefaultsOnly, Category="Item Inventory Config")
	EItemInventoryType InventoryType;
	UPROPERTY(EditDefaultsOnly, Category="Item Inventory Config")
	bool Equippable;
	UPROPERTY(EditAnywhere, Category="Item Inventory Config")
	EUnitAnimArmsMode EquippedAnimArmsMode;

protected:
	IItemHolder* CurrentHolder;

private:
	UPROPERTY(EditDefaultsOnly, Category="Item")
	UStaticMeshComponent* VisibleComponent;
	UPROPERTY(EditDefaultsOnly, Category="Item")
	UBoxComponent* ColliderComponent;

public:
	AItemActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	UItemContainerComponent* ItemAsContainer();
	virtual void ItemTake(IItemHolder* Target);
	virtual void ItemDrop(IItemHolder* Target);
};
