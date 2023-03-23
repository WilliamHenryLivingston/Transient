#include "UnitSlotComponent.h"

#include "EquippedMeshConfig.h"
#include "ItemHolder.h"

UUnitSlotComponent::UUnitSlotComponent() {
	PrimaryComponentTick.bCanEverTick = true;

	this->SetSimulatePhysics(false);
	this->SetCollisionProfileName(FName("NoCollision"), true);
}

void UUnitSlotComponent::BeginPlay() {
	Super::BeginPlay();

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
		this->SetStaticMesh(nullptr);
	}
	else {
		FEquippedMeshConfig Config = this->Content->EquippedMesh;
		this->SetStaticMesh(Config.Mesh);
		this->SetRelativeScale3D(Config.Scale);
		this->SetRelativeRotation(Config.AltRotation);
	}
}
