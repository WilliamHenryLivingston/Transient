#include "UnitSlotComponent.h"

#include "EquippedMeshConfig.h"

UUnitSlotComponent::UUnitSlotComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UUnitSlotComponent::BeginPlay() {
	Super::BeginPlay();
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
