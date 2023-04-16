// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "MagazineItem.h"

void AMagazineItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMagazineItem, this->Ammo);
}

int AMagazineItem::MagazineAmmo() { return this->Ammo; }

void AMagazineItem::MagazineDeplete(int Amount) {
	this->Ammo -= Amount;
	if (this->Ammo <= 0) {
		this->Destroy();
		return;
	}

	this->MagazineAmmoChanged();
}

void AMagazineItem::MagazineAmmoChanged() {	
	int StateCount = this->StateVariants.Num();
	if (StateCount == 0) return;

	int Stop = this->Capacity / StateCount;
	for (int i = StateCount - 1; i >= 0; i--) {
		if (this->Ammo >= (i * Stop)) {
			this->VisibleComponent->SetStaticMesh(this->StateVariants[i]);
			break;
		}
	}
}
