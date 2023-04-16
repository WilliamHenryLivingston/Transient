// Copyright: R. Saxifrage, 2023. All rights reserved.

// Items can be placed in inventory slots (via attachment), and used in various ways by
// their parent unit actor. Items are either in the world or in a slot.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

#include "Transient/Animation/UnitAnimInstance.h"
#include "Transient/Rep/ReplicatedSoundComponent.h"
#include "Transient/Rep/ReplicatedNiagaraComponent.h"

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
	MiscLarge,
	MuzzleAttachment,
	MainRailAttachment,
	SecondaryRailAttachment
};

USTRUCT(BlueprintType)
struct FItemInfo {
public:
	FString Name;
	FString Details;
};

class UInventorySlotComponent;
class AUnitAgent;

UCLASS()
class AItemActor : public AActor {
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
	bool TakeToHandsIfPossible;
	UPROPERTY(EditDefaultsOnly, Category="Item Inventory Config")
	bool EquipStateOnly;
	UPROPERTY(EditDefaultsOnly, Category="Item Inventory Config")
	float EquippedTorsoYaw;
	UPROPERTY(EditDefaultsOnly, Category="Item Inventory Config")
	bool EquipAltHand;
	UPROPERTY(EditDefaultsOnly, Category="Item Inventory Config")
	EUnitAnimArmsState EquippedAnimArmsMode;
	UPROPERTY(EditDefaultsOnly, Category="Item Inventory Config")
	FString WorldCollisionProfile;
	UPROPERTY(EditDefaultsOnly, Category="Item Inventory Config")
	FString ItemName;
	UPROPERTY(EditDefaultsOnly, Category="Item Inventory Config")
	int EquippedConcealment;

	UPROPERTY(EditDefaultsOnly, Category="Item Usability")
	bool Usable;
	UPROPERTY(EditDefaultsOnly, Category="Item Usability")
	TSubclassOf<AActor> TargetType;
	UPROPERTY(EditDefaultsOnly, Category="Item Usability")
	bool RequiresTarget;
	UPROPERTY(EditDefaultsOnly, Category="Item Usability")
	bool ImmobilizeOnUse;
	UPROPERTY(EditDefaultsOnly, Category="Item Usability")
	FAnimationConfig UseAnimation;

protected:
	// Isomorphic.
	UPROPERTY(EditDefaultsOnly, Category="Item")
	UStaticMeshComponent* VisibleComponent;
	UPROPERTY(EditDefaultsOnly, Category="Item")
	UBoxComponent* ColliderComponent;

	UReplicatedNiagaraComponent* UseNiagara;
	UReplicatedSoundComponent* UseSound;

private:
	UPROPERTY(ReplicatedUsing=ItemSlotNameChanged)
	FString SlotName;
	UPROPERTY(Replicated)
	AActor* Holder;

	// Isomorphic, non-replicated.
	UInventoryComponent* Slot;

public:
	AItemActor();
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// Isomorphic.
	void ItemSlotNameChanged();

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	virtual FItemInfo ItemInfo();
	UInventorySlotComponent* ItemSlot();

	// Game logic.
	void ItemStageSlotChange(UInventorySlotComponent* Slot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ItemStartUse();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ItemUse(AActor* Target);

protected:
	// Isomorphic.
	AActor* ItemHolder();
	AUnitAgent* ItemHolderUnit();

	void ItemPlaySoundOnce(USoundBase* Sound);
};
