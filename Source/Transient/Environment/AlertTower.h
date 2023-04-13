// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "../AI/AIGroup.h"
#include "../Animation/BooleanAnimInstance.h"
#include "InteractiveActor.h"

#include "AlertTower.generated.h"

UCLASS()
class AAlertTower : public AInteractiveActor, public IAIGroupMember {
    GENERATED_BODY()

private:
    TArray<AAIGroup*> Groups;
    UBooleanAnimInstance* Animation;

protected:
    virtual void BeginPlay() override;

public:
    virtual void InteractiveUse(AActor* User) override;

public:
	virtual void AIGroupMemberJoin(AAIGroup* Group) override;
};
