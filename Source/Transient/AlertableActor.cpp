#include "AlertableActor.h"

AAlertableActor::AAlertableActor() {
	PrimaryActorTick.bCanEverTick = true;
}

void AAlertableActor::BeginPlay() {
	Super::BeginPlay();

	this->Animation = Cast<UAlertableAnimInstance>(this->FindComponentByClass<USkeletalMeshComponent>()->GetAnimInstance());

	TArray<USceneComponent*> SceneComponents;
	this->GetComponents(SceneComponents, true);
	for (int i = 0; i < SceneComponents.Num(); i++) {
		USceneComponent* Check = SceneComponents[i];

		FString Name = Check->GetName();
		if (Name.Contains("ActiveOnly")) {
			this->ActiveOnlyComponents.Push(Check);
			Check->SetVisibility(false);
		}
	}
}

void AAlertableActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AAlertableActor::AIGroupMemberAlert(AActor* AgroTarget) {
	this->Animation->Script_Alerted = true;

	for (int i = 0; i < this->ActiveOnlyComponents.Num(); i++) {
		this->ActiveOnlyComponents[i]->SetVisibility(true);
	}
}
