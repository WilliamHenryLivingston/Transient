// Copyright: R. Saxifrage, 2023. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"

#include "ItemActor.h"

#include "LaserAttachmentItem.generated.h"

UCLASS()
class ALaserAttachmentItem : public AItemActor {
	GENERATED_BODY()

public:
	ALaserAttachmentItem();
	virtual void Tick(float DeltaTime) override;
};
