// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "AIGroup.h"
#include "AlertableAnimInstance.h"

#include "AlertableActor.generated.h"

UCLASS()
class TRANSIENT_API AAlertableActor : public AActor, public IAIGroupMember {
	GENERATED_BODY()

private:
	TArray<USceneComponent*> ActiveOnlyComponents;
	UAlertableAnimInstance* Animation;

// AActor.
public:	
	AAlertableActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

// IAIGroupMember.
public:
	virtual void AIGroupMemberAlert(AActor* AgroTarget) override;
};
