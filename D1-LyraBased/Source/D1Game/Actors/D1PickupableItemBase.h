#pragma once

#include "Interaction/D1WorldPickupable.h"
#include "D1PickupableItemBase.generated.h"

class UBoxComponent;
class UArrowComponent;
class UProjectileMovementComponent;

UCLASS()
class AD1PickupableItemBase : public AD1WorldPickupable
{
	GENERATED_BODY()
	
public:
	AD1PickupableItemBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void OnRep_PickupInfo() override;
	virtual void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const override;
	
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UArrowComponent> ArrowComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> MeshComponent;
};
