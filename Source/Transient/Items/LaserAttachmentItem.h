// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"

#include "ItemActor.h"

#include "LaserAttachmentItem.generated.h"

UCLASS()
class TRANSIENT_API ALaserAttachmentItem : public AItemActor {
	GENERATED_BODY()

private:
	UNiagaraComponent* LaserFX;

public:
	ALaserAttachmentItem();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
};
