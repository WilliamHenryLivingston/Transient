#include "KeycardItem.h"

#include "../Environment/KeyLock.h"

AKeycardItem::AKeycardItem() {
    this->TargetType = AKeyLock::StaticClass();
    this->RequiresTarget = true;
}

void AKeycardItem::ItemUse(AActor* Target) {
    AKeyLock* AsLock = Cast<AKeyLock>(Target);

    if (AsLock->LockID == this->LockID) AsLock->LockSwipeValid();
    else AsLock->LockSwipeInvalid();
}
