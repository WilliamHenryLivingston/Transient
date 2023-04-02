#pragma once

#include "CoreMinimal.h"

#include "InteractiveActor.h"
#include "ProgressUIWidget.h"

#include "ProgressUIInteractive.generated.h"

UCLASS()
class TRANSIENT_API AProgressUIInteractive : public AInteractiveActor {
	GENERATED_BODY()

protected:
	UProgressUIWidget* ProgressUI;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;	
};
