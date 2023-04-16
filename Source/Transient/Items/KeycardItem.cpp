// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "KeycardItem.h"

#include "Transient/Environment/KeyLock.h"

void AKeycardItem::ItemUse_Implementation(AActor* Target) {
    Super::ItemUse_Implementation(Target);

    AKeyLock* AsLock = Cast<AKeyLock>(Target);

    if (AsLock->LockID == this->LockID) AsLock->LockSwipeValid();
    else AsLock->LockSwipeInvalid();
}
