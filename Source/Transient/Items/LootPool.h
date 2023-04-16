// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ItemActor.h"

#include "LootPool.generated.h"

USTRUCT()
struct FLootPoolEntry {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AItemActor> Item;
	UPROPERTY(EditAnywhere)
	float Chance;
};

UCLASS(Blueprintable)
class ULootPool : public UObject {
    GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly, Category="Loot Pool")
    TArray<FLootPoolEntry> Entries;

public:
    TArray<TSubclassOf<AItemActor>> LootPoolEvaluate();
};
