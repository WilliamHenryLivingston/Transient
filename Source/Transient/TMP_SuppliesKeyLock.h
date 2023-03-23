#pragma once

#include "CoreMinimal.h"
#include "KeyLock.h"
#include "TMP_SuppliesKeyLock.generated.h"

UCLASS()
class TRANSIENT_API ATMP_SuppliesKeyLock : public AKeyLock
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SuppliesType;

	virtual void LockSwipeInvalid() override;
	virtual void LockSwipeValid() override;
};
