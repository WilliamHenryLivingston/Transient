#include "MagazineItem.h"

AMagazineItem::AMagazineItem() {
	this->PrimaryActorTick.bCanEverTick = true;

	this->EquipType = EItemEquipType::Magazine;
}