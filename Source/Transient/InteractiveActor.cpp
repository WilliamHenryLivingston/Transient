#include "InteractiveActor.h"

AInteractiveActor::AInteractiveActor() {
	PrimaryActorTick.bCanEverTick = true;

}

void AInteractiveActor::BeginPlay() {
	Super::BeginPlay();
	
	TArray<USceneComponent*> SceneComponents;
	this->GetComponents(SceneComponents, true);
	for (int i = 0; i < SceneComponents.Num(); i++) {
		USceneComponent* Check = SceneComponents[i];

		FString Name = Check->GetName();
		if (Name.Equals("InteractLookTarget")) this->InteractLookTargetComponent = Check;
	}
}

void AInteractiveActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void AInteractiveActor::InteractiveUse(AActor* User) {
	for (int i = 0; i < this->AlertOnUse.Num(); i++) {
		this->AlertOnUse[i]->AIGroupDistributeAlert(User);
	}

	if (this->OpenOnUse != nullptr) {
		this->OpenOnUse->DoorSetLocked(false);
		this->OpenOnUse->DoorSetOpen(true);
	}
}
