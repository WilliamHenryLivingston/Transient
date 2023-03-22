#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

#include "ItemHolder.h"
#include "EquippedMeshConfig.h"

#include "ItemActor.generated.h"

UCLASS()
class TRANSIENT_API AItemActor : public AActor {
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere)
	FEquippedMeshConfig EquippedMesh;

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
	virtual void ItemEquip(IItemHolder* Target);

	virtual void ItemDequip(IItemHolder* Target);
	
	void ItemDestroy();
};
