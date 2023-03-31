#include "InteractiveActor.h"

AInteractiveActor::AInteractiveActor() {
	PrimaryActorTick.bCanEverTick = true;

}

void AInteractiveActor::BeginPlay() {
	Super::BeginPlay();
	
}

void AInteractiveActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void AInteractiveActor::InteractiveUse(AActor* User) {

}
