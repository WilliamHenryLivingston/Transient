// Fill out your copyright notice in the Description page of Project Settings.


#include "TMP_SuppliesKeyLock.h"


void ATMP_SuppliesKeyLock::LockSwipeInvalid() { return; }

void ATMP_SuppliesKeyLock::LockSwipeValid() {
    this->GetWorld()->SpawnActor<AActor>(
        this->SuppliesType,
        this->GetActorLocation() + (this->GetActorForwardVector() * 100.0f),
        FRotator(),
        FActorSpawnParameters()
    );
}
