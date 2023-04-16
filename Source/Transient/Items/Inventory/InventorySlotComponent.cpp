// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "InventorySlotComponent.h"

#include "Components/SphereComponent.h"

#include "Items/ItemHolder.h"

void UInventorySlotComponent::BeginPlay() {
	Super::BeginPlay();

	this->ParentInventory = this->GetOwner()->FindComponentByClass<UInventoryComponent>();
}

FString UInventorySlotComponent::SlotUniqueName() { return this->GetName(); } // TODO: Always works?

UInventoryComponent* UInventorySlotComponent::SlotInventory() { return this->ParentInventory; }

TArray<EItemInventoryType> UInventorySlotComponent::SlotAllowedTypes() { return this->AllowedItems; }

AItemActor* UInventorySlotComponent::SlotContent() { return this->Content; }

void UInventorySlotComponent::SlotSetContent(AItemActor* NewContent) {
	// This flow is indirect because the item necessarily (TODO: ?) holds the replicated prop.
	// SlotSetContent (all game logic) ->
	// Item::ItemStageSlotChange (rep'd prop update) ->
	// Item::ItemSlotNameChanged (rep callback) ->
	// SlotFinallyBindContent (attachment setup)

	if (NewContent == this->Content) return;

	if (this->Content != nullptr) this->Content->ItemSetSlotFromSlot(nullptr);

	if (NewContent != nullptr) {
		UInventorySlotComponent* PrevSlot = NewContent->ItemSlot();
		if (PrevSlot != nullptr) PrevSlot->Content = nullptr;

		NewContent->ItemSetSlotFromSlot(this);
	}
}

void UInventorySlotComponent::SlotFinallyBindContent(AItemActor* NewContent) {
	this->Content = NewContent;
	if (this->Content == nullptr) return;

	this->Content->AttachToComponent(
		this,
		FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			true
		),
		FName("None")
	);
	this->Content->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
	this->Content->SetActorRelativeLocation(
		this->UseEquippedItemTransform ? this->Content->EquippedOffset : this->Content->InSlotOffset
	);
	this->Content->SetActorRelativeRotation(
		this->UseEquippedItemTransform ? this->Content->EquippedRotation : this->Content->InSlotRotation
	);
}
