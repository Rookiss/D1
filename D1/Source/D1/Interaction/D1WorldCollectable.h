#pragma once

#include "D1Interactable.h"
#include "D1InteractionInfo.h"
#include "D1Pickupable.h"
#include "D1WorldCollectable.generated.h"

class UD1ItemInstance;

UCLASS(Abstract, Blueprintable)
class AD1WorldCollectable : public AActor, public ID1Interactable, public ID1Pickupable
{
	GENERATED_BODY()
	
public:
	AD1WorldCollectable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	// virtual void GatherInteractionInfos();
	
protected:
	UPROPERTY(EditAnywhere)
	FD1InteractionInfo InteractionInfo;

	UPROPERTY(EditAnywhere)
	FPickupInfo PickupInfo;
};
