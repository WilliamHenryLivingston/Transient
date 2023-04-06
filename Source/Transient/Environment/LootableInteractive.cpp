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

	this->Animation = Cast<UBooleanAnimInstance>(this->FindComponentByClass<USkeletalMeshComponent>()->GetAnimInstance());
	this->Animation->Script_Flag = false;
}

void ALootableInteractive::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ALootableInteractive::InteractiveUse(AActor* User) {
	Super::InteractiveUse(User);

	if (!this->Animation->Script_Flag) {
		this->Animation->Script_Flag = true;
		this->ClosedStateColliderComponent->SetCollisionProfileName(FName("NoCollision"));
	}
	else {
		this->Animation->Script_Flag = false;
		this->ClosedStateColliderComponent->SetCollisionProfileName(FName("LootableClosed"));
	}
}
