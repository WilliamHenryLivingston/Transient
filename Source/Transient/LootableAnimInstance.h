#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "LootableAnimInstance.generated.h"

UENUM(BlueprintType)
enum class ELootableAnimState : uint8 {
    Closed,
    Open
};

UCLASS(Transient, Blueprintable, HideCategories=AnimInstance, BlueprintType)
class ULootableAnimInstance : public UAnimInstance {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Script Controlled")
    ELootableAnimState Script_State;

public:
	ULootableAnimInstance();
};
