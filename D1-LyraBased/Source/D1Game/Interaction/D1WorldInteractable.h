#pragma once

#include "D1Interactable.h"
#include "D1WorldInteractable.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class AD1WorldInteractable : public AActor, public ID1Interactable
{
	GENERATED_BODY()
	
public:
	AD1WorldInteractable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual FD1InteractionInfo GetPreInteractionInfo(const FD1InteractionQuery& InteractionQuery) const override { return FD1InteractionInfo(); }
};
