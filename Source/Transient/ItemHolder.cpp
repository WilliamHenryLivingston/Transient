#include "ItemHolder.h"

FVector IItemHolder::ItemHolderGetLocation() { return FVector(); }
FRotator IItemHolder::ItemHolderGetRotation() { return FRotator(); }
void IItemHolder::ItemHolderPlaySound(USoundBase* Target) { return; }
float IItemHolder::ItemHolderGetSpreadModifier() { return 1.0f; }
