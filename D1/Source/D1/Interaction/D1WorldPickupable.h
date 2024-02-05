#pragma once

#include "D1Pickupable.h"
#include "D1WorldInteractable.h"
#include "D1WorldPickupable.generated.h"

UCLASS(Abstract, Blueprintable)
class AD1WorldPickupable : public AD1WorldInteractable, public ID1Pickupable
{
	GENERATED_BODY()
	
public:
	AD1WorldPickupable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual const FPickupInfo& GetPickupInfo() const override { return PickupInfo; }
	
protected:
	UPROPERTY(EditAnywhere)
	FPickupInfo PickupInfo;
};
