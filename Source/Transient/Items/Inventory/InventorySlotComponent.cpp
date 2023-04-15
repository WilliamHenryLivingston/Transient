// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "InventorySlotComponent.h"

#include "Components/SphereComponent.h"

#include "Items/ItemHolder.h"

void UInventorySlotComponent::BeginPlay() {
	Super::BeginPlay();

	this->ParentInventory = this->GetOwner()->FindComponentByClass<UInventoryComponent>();
}

UInventoryComponent* UInventorySlotComponent::SlotInventory() { return this->ParentInventory; }

TArray<EItemInventoryType> UInventorySlotComponent::SlotAllowedTypes() { return this->AllowedItems; }

AItemActor* UInventorySlotComponent::SlotContent() { return this->Content; }

void UInventorySlotComponent::SlotSetContentReplicated(AItemActor* NewContent) {
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

void UInventorySlotComponent::SlotSetContent(AItemActor* NewContent) {
	if (this->Content != nullptr) {
		this->Content->ItemSetSlot(nullptr);
	}

	this->Content = NewContent;
	if (this->Content != nullptr) {
		this->Content->ItemSetSlot(this);
	}
}
