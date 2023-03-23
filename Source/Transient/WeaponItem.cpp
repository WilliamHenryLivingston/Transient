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

void AWeaponItem::WeaponSetTriggerPulled(bool NewTriggerPulled) {
	this->TriggerPulled = NewTriggerPulled;
}

// Stubs.
void AWeaponItem::WeaponSwapMagazines(int NewAmmoCount) { return; }
bool AWeaponItem::WeaponEmpty() { return false; }