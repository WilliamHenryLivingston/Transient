#include "MagazineItem.h"

AMagazineItem::AMagazineItem() {
	this->PrimaryActorTick.bCanEverTick = true;

	this->InventoryType = EItemInventoryType::Magazine;
}