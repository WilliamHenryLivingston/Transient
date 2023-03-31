#include "LootableInteractive.h"

ALootableInteractive::ALootableInteractive() {
	PrimaryActorTick.bCanEverTick = true;
}

void ALootableInteractive::BeginPlay() {
	Super::BeginPlay();

	TArray<UBoxComponent*> BoxComponents;
	this->GetComponents(BoxComponents, true);
	for (int i = 0; i < BoxComponents.Num(); i++) {
		UBoxComponent* Check = BoxComponents[i];

		FString Name = Check->GetName();
		if (Name.Equals("ClosedStateCollider")) this->ClosedStateColliderComponent = Check;
	}

	this->Animation = Cast<ULootableAnimInstance>(this->FindComponentByClass<USkeletalMeshComponent>()->GetAnimInstance());
	this->Animation->Script_State = ELootableAnimState::Closed;
}

void ALootableInteractive::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ALootableInteractive::InteractiveUse(AActor* User) {
	if (this->Animation->Script_State == ELootableAnimState::Closed) {
		this->Animation->Script_State = ELootableAnimState::Open;
		this->ClosedStateColliderComponent->SetCollisionProfileName(FName("NoCollision"));
	}
	else {
		this->Animation->Script_State = ELootableAnimState::Closed;
		this->ClosedStateColliderComponent->SetCollisionProfileName(FName("LootableClosed"));
	}
}
