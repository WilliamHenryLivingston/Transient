#include "WeaponItem.h"

#include "ProjectileActor.h"

#include "Components/BoxComponent.h"

AWeaponItem::AWeaponItem() {
	this->PrimaryActorTick.bCanEverTick = true;

	this->InventoryType = EItemInventoryType::Weapon;
	this->Equippable = true;
}

void AWeaponItem::BeginPlay() {
	Super::BeginPlay();	
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

FVector AWeaponItem::WeaponGetRelativeMuzzleAsEquipped() {
	if (this->CurrentHolder == nullptr) return FVector(); // Invalid call.

	FVector LocalMuzzle = this->CurrentHolder->ItemHolderGetWeaponOffset() + this->MuzzleLocation;
	return this->CurrentHolder->ItemHolderGetRotation().RotateVector(LocalMuzzle);
}

// Stubs.
void AWeaponItem::WeaponSwapMagazines(int NewAmmoCount) { return; }
bool AWeaponItem::WeaponEmpty() { return false; }