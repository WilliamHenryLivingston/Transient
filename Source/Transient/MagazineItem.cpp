#include "MagazineItem.h"

AMagazineItem::AMagazineItem() {
	this->PrimaryActorTick.bCanEverTick = true;

	this->InventoryType = EItemInventoryType::Magazine;
}

void AMagazineItem::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	int StateCount = this->StateVariants.Num();
	if (StateCount > 0) {
		int Stop = this->Capacity / StateCount;
		for (int i = StateCount - 1; i >= 0; i--) {
			if (this->Ammo >= (i * Stop)) {
				this->VisibleComponent->SetStaticMesh(this->StateVariants[i]);
				break;
			}
		}
	}
}
