// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "InteractiveAgent.h"

#include "Transient/Common.h"

void AInteractiveAgent::BeginPlay() {
	Super::BeginPlay();

	TArray<USceneComponent*> SceneComponents;
	this->GetComponents(SceneComponents, true);
	for (int i = 0; i < SceneComponents.Num(); i++) {
		USceneComponent* Check = SceneComponents[i];

		FString Name = Check->GetName();
		if (Name.Equals("InteractLookTarget")) this->InteractLookTargetComponent = Check;
	}
}

void AInteractiveAgent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AInteractiveAgent, this->Available);
}

bool AInteractiveAgent::InteractiveAvailable() { return this->Available; }

void AInteractiveAgent::InteractiveUse_Implementation(AUnitAgent* User) {
	ERR_LOG(this, "unhandled interact");
}
