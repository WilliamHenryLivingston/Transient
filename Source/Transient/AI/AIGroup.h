// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "../DebugViewActor.h"

#include "AIGroup.generated.h"

UINTERFACE()
class UAIGroupMember : public UInterface {
	GENERATED_BODY()
};

class IAIGroupMember {
	GENERATED_BODY()

public:
	virtual void AIGroupMemberJoin(AAIGroup* Group);
	virtual void AIGroupMemberAlert(AActor* AgroTarget);
};

UCLASS()
class TRANSIENT_API AAIGroup : public ADebugViewActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Group")
	TArray<AActor*> InitialMembers;
	UPROPERTY(EditAnywhere, Category="Group")
	TArray<AAIGroup*> AlertChain;
	UPROPERTY(EditAnywhere, Category="Group")
	TArray<AActor*> AlertTowers;

	TArray<IAIGroupMember*> Members;

public:	
	AAIGroup();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	void AIGroupDistributeAlert(AActor* Target);
};