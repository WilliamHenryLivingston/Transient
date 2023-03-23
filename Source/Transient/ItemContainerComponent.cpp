#include "ItemContainerComponent.h"

UItemContainerComponent::UItemContainerComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UItemContainerComponent::BeginPlay() {
	Super::BeginPlay();
}

void UItemContainerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
