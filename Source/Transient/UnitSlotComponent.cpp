#include "UnitSlotComponent.h"

#include "Components/SphereComponent.h"

#include "TransientDebug.h"
#include "EquippedMeshConfig.h"
#include "ItemHolder.h"

UUnitSlotComponent::UUnitSlotComponent() {
	PrimaryComponentTick.bCanEverTick = true;

	this->SetSimulatePhysics(false);
	this->SetCollisionProfileName(FName("NoCollision"), true);
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
		this->SetStaticMesh(nullptr);
	}
	else {
		FEquippedMeshConfig Config = this->Content->EquippedMesh;
		this->SetStaticMesh(Config.Mesh);
		this->SetRelativeScale3D(Config.Scale);
		if (Config.Scale.X > 0.1f) {
			this->InventoryLookCollider->SetSphereRadius(this->InventoryViewColliderRadius * (1.0f / Config.Scale.X));
		}
		else {
			this->InventoryLookCollider->SetSphereRadius(0.0f);
		}
		this->SetRelativeRotation(Config.AltRotation);
	}
}
