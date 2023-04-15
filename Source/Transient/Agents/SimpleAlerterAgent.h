// Copyright: R. Saxifrage, 2023. All rights reserved.

// An agent that distributes an alert when interacted with, and has a
// corresponding boolean animation state.

// TODO: Into blueprint.

#pragma once

#include "CoreMinimal.h"

#include "Transient/Animation/BooleanAnimInstance.h"

#include "InteractiveAgent.h"

#include "AlerterAgent.generated.h"

UCLASS()
class ASimpleAlerterAgent : public AInteractiveAgent {
    GENERATED_BODY()

private:
    UPROPERTY(ReplicatedUsing=AlerterOnActivatedChanged)
    bool Activated;

    UBooleanAnimInstance* Animation;

public:
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    virtual void BeginPlay() override;

protected:
    // Cosmetic.
	virtual void InteractiveAvailableChanged() override;

public:
    // Game logic.
    virtual void InteractiveUse_Implementation(AUnitAgent* User) override;
};
