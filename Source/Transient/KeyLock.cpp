#include "KeyLock.h"

AKeyLock::AKeyLock() {
	PrimaryActorTick.bCanEverTick = true;
}

void AKeyLock::BeginPlay() {
	Super::BeginPlay();
}

void AKeyLock::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AKeyLock::LockSwipeInvalid() { return; }
void AKeyLock::LockSwipeValid() { return; }
