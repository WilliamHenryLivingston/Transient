// Copyright: R. Saxifrage, 2023. All rights reserved.

#include "EffectActor.h"

AEffectActor::AEffectActor() {
    PrimaryActorTick.bCanEverTick = true;
    this->SetReplicates(true);

    this->SoundComponent = this->CreateDefaultSubobject<UAudioComponent>(TEXT("SoundEmitter"));
    this->SoundComponent->SetupAttachment(this->RootComponent);

    this->NiagaraComponent = this->CreateDefaultSubobject<UNiagaraComponent>(TEXT("ParticleEmitter"));
    this->NiagaraComponent->SetupAttachment(this->RootComponent);
}

void AEffectActor::BeginPlay() {
    this->NiagaraComponent->SetAsset(this->EffectNiagara, true);

    this->SoundComponent->Sound = this->EffectSound;
    this->SoundComponent->Play(0.0f);
}

void AEffectActor::Tick(float DeltaTime) {
    this->LifeTimer += DeltaTime;

    if (this->LifeTimer > this->LifeTime) this->Destroy();
}
