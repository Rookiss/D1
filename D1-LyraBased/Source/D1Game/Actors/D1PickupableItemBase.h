#pragma once

#include "Interaction/D1WorldPickupable.h"
#include "D1PickupableItemBase.generated.h"

class UArrowComponent;

UCLASS()
class AD1PickupableItemBase : public AD1WorldPickupable
{
	GENERATED_BODY()
	
public:
	AD1PickupableItemBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void InitializeActor(UD1ItemInstance* InItemInstance, int32 InItemCount);
	
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UArrowComponent> ArrowComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> ItemMeshComponent;
};
