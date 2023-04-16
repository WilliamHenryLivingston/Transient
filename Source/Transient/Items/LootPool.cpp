// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "LootPool.h"

TArray<TSubclassOf<AItemActor>> LootPoolEvaluate() {
    TArray<TSubclassOf<AItemActor>> Result;

    for (int i = 0; i < this->Entries.Num(); i++) {
        FLootPoolEntry Entry = this->Entries[i];

        if (FMath::RandRange(0.0f, 1.0f) < Entry.Chance) {
            Result.Push(Entry.Item);
        }
    }

    return Result;
}
