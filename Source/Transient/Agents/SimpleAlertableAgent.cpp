// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "SimpleAlertableAgent.h"

void ASimpleAlertableAgent::BeginPlay() {
	Super::BeginPlay();

	USkeletalMeshComponent* Skeleton = this->FindComponentByClass<USkeletalMeshComponent>();
	this->Animation = Cast<UBooleanAnimInstance>(Skeleton->GetAnimInstance());
}

void ASimpleAlertableAgent::AgentHasTargetsChanged_Implementation() {
	Super::AgentHasTargetsChanged();

	this->Animation->Script_Flag = this->HasTargets;
}
