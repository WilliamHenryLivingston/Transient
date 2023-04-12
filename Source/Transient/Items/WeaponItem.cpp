// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "WeaponItem.h"

#include "MagazineItem.h"

#include "Components/BoxComponent.h"

AWeaponItem::AWeaponItem() {
	this->PrimaryActorTick.bCanEverTick = true;

	this->InventoryType = EItemInventoryType::Weapon;
	this->Equippable = true;
}

void AWeaponItem::BeginPlay() {
	Super::BeginPlay();
	
	TArray<USceneComponent*> SceneComponents;
	this->GetComponents(SceneComponents, true);
	for (int i = 0; i < SceneComponents.Num(); i++) {
		USceneComponent* Check = SceneComponents[i];

		FString Name = Check->GetName();
		if (Name.Equals("MuzzlePosition")) this->MuzzlePosition = Check;
		else if (Name.Equals("ActiveMagazineHost")) this->ActiveMagazineHost = Check;
	}

	this->GetComponents(this->AttachmentSlots, true);
}

void AWeaponItem::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

bool AWeaponItem::WeaponGetTriggerPulled() {
	return this->TriggerPulled;
}

void AWeaponItem::WeaponSetTriggerPulled(bool NewTriggerPulled) {
	this->TriggerPulled = NewTriggerPulled;
}

FVector AWeaponItem::WeaponGetMuzzlePosition() {
	return this->MuzzlePosition->GetComponentLocation();
}

FRotator AWeaponItem::WeaponGetMuzzleRotation() {
	return this->MuzzlePosition->GetComponentRotation();
}

AMagazineItem* AWeaponItem::WeaponGetMagazine() {
	return this->ActiveMagazine;
}

void AWeaponItem::WeaponSwapMagazines(AMagazineItem* NewMagazine) {
	this->WeaponDisposeCurrentMagazine();

	this->ActiveMagazine = NewMagazine;
	if (this->ActiveMagazine == nullptr) return;

	this->CurrentHolder->ItemHolderPlaySound(this->ReloadSound);

	this->ActiveMagazine->AttachToComponent(
		this->ActiveMagazineHost,
		FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			true
		),
		FName("None")
	);
}

bool AWeaponItem::WeaponHasItemEquipped(AItemActor* Item) {
	for (int i = 0; i < this->AttachmentSlots.Num(); i++) {
		AItemActor* Check = this->AttachmentSlots[i]->SlotGetContent();
		if (Check == Item) return true;
	}

	return false;
}

void AWeaponItem::WeaponDisposeCurrentMagazine() {
	if (this->ActiveMagazine == nullptr) return;

	if (this->ActiveMagazine->Ammo > 0) {
		this->ActiveMagazine->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
		this->ActiveMagazine->ItemDrop(Cast<AActor>(this->CurrentHolder));
	}
	else {
		this->ActiveMagazine->Destroy();
	}
	this->ActiveMagazine = nullptr;
}

bool AWeaponItem::WeaponEmpty() {
	return this->ActiveMagazine == nullptr || this->ActiveMagazine->Ammo == 0;
}