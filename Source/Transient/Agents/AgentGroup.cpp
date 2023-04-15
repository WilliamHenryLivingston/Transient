// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "AIGroup.h"

void IAIGroupMember::AIGroupMemberJoin(AAIGroup* Group) { return; }
void IAIGroupMember::AIGroupMemberAlert(AActor* AgroTarget) { return; }

AAIGroup::AAIGroup() {
	PrimaryActorTick.bCanEverTick = true;
}

void AAIGroup::BeginPlay() {
	Super::BeginPlay();

	for (int i = 0; i < this->InitialMembers.Num(); i++) {
		IAIGroupMember* AsMember = Cast<IAIGroupMember>(this->InitialMembers[i]);
		if (AsMember == nullptr) continue;

		this->Members.Push(AsMember);
		AsMember->AIGroupMemberJoin(this);
	}

	this->InitialMembers = TArray<AActor*>();
}

void AAIGroup::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	for (int i = 0; i < this->Members.Num(); i++) {
		if (!IsValid(Cast<AActor>(this->Members[i]))) {
			this->Members.RemoveAt(i);
			i--;
		}
	}
}

void AAIGroup::AIGroupDistributeAlert(AActor* Target) {
	for (int i = 0; i < this->Members.Num(); i++) {
		this->Members[i]->AIGroupMemberAlert(Target);
	}

	for (int i = 0; i < this->AlertChain.Num(); i++) {
		this->AlertChain[i]->AIGroupDistributeAlert(Target);
	}
}
