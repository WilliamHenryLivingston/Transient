// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "DoorActor.h"

ADoorActor::ADoorActor() {
	PrimaryActorTick.bCanEverTick = true;
}

void ADoorActor::BeginPlay() {
	Super::BeginPlay();

	this->Animation = Cast<UBooleanAnimInstance>(this->FindComponentByClass<USkeletalMeshComponent>()->GetAnimInstance());
}

void ADoorActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

bool ADoorActor::DoorIsLocked() {
	return this->Locked;
}

void ADoorActor::DoorSetOpen(bool NewOpen) {
	if (this->Locked) return;

	this->Open = NewOpen;
	this->Animation->Script_Flag = this->Open;
}

void ADoorActor::DoorSetLocked(bool NewLocked) {
	this->Locked = NewLocked;
}
