#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

#include "ItemHolder.h"
#include "EquippedMeshConfig.h"

#include "ItemActor.generated.h"

UENUM(BlueprintType)
enum class EItemEquipType : uint8 {
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
	UPROPERTY(EditAnywhere, Category="Item Equip Config")
	FEquippedMeshConfig EquippedMesh;
	UPROPERTY(EditDefaultsOnly, Category="Item Equip Config")
	EItemEquipType EquipType;

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
	virtual void ItemEquip(IItemHolder* Target);
	virtual void ItemDequip(IItemHolder* Target);
};
