#include "UnitSlotComponent.h"

#include "Components/SphereComponent.h"

#include "TransientDebug.h"
#include "ItemHolder.h"

UUnitSlotComponent::UUnitSlotComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UUnitSlotComponent::BeginPlay() {
	Super::BeginPlay();
	
	// TODO: Not here?
	this->InventoryLookCollider = NewObject<UUnitSlotColliderComponent>(this->GetOwner());
	this->InventoryLookCollider->RegisterComponent();
	this->InventoryLookCollider->AttachToComponent(
		this,
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		FName("None")
	);
	this->InventoryLookCollider->SetSphereRadius(0.0f);
	this->InventoryLookCollider->SetCollisionProfileName(FName("EquipHost"));
	this->InventoryLookCollider->ParentSlot = this;
	this->InventoryLookCollider->SetHiddenInGame(NODEBUG_COLLIDERS);

	if (this->Content != nullptr) {
		this->Content->ItemTake(Cast<IItemHolder>(this->GetOwner()));
		this->SlotSetContent(this->Content);
	}
}

void UUnitSlotComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

AItemActor* UUnitSlotComponent::SlotGetContent() {
	return this->Content;
}

void UUnitSlotComponent::SlotSetContent(AItemActor* NewContent) {
	this->Content = NewContent;

	if (this->Content == nullptr) {
		this->InventoryLookCollider->SetSphereRadius(0.0f);
	}
	else {
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
		this->Content->SetActorRelativeLocation(this->Content->InSlotOffset);
		this->Content->SetActorRelativeRotation(this->Content->InSlotRotation);
		this->InventoryLookCollider->SetSphereRadius(this->InventoryViewColliderRadius * this->Content->SlotColliderModifier);
	}
}
