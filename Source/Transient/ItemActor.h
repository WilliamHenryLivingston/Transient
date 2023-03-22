#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

#include "ItemHolder.h"

#include "ItemActor.generated.h"

UCLASS()
class TRANSIENT_API AItemActor : public AActor {
	GENERATED_BODY()
	
public:
	static TArray<AItemActor*> WorldItems;

protected:
	IItemHolder* CurrentHolder;

private:
	UPROPERTY(EditAnywhere)
	UBoxComponent* ColliderComponent;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* VisibleComponent;

public:	
	AItemActor();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	static TArray<AItemActor*> ItemsGetNearby(FVector Location, float Reach);

	virtual void ItemEquip(IItemHolder* Target);

	virtual void ItemDequip(IItemHolder* Target);
	
	void ItemDestroy();
};
