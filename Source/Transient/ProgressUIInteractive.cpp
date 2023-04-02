#include "ProgressUIInteractive.h"

#include "Components/WidgetComponent.h"

void AProgressUIInteractive::BeginPlay() {
    Super::BeginPlay();

    this->ProgressUI = Cast<UProgressUIWidget>(this->FindComponentByClass<UWidgetComponent>()->GetUserWidgetObject());
}

void AProgressUIInteractive::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if (this->InteractTimer < 0.0f) {
        this->ProgressUI->Script_Progress = 0.0f;
        this->ProgressUI->Script_InProgress = false;
        return;
    }

    this->ProgressUI->Script_InProgress = true;
    this->ProgressUI->Script_Progress = this->InteractTimer / this->InteractTime;
}
