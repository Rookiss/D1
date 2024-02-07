#pragma once

#include "D1Interactable.h"
#include "D1WorldInteractable.generated.h"

UCLASS(Abstract, Blueprintable)
class AD1WorldInteractable : public AActor, public ID1Interactable
{
	GENERATED_BODY()
	
public:
	AD1WorldInteractable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
public:
	virtual const FD1InteractionInfo& GetInteractionInfo() const { return InteractionInfo; }

protected:
	UPROPERTY(EditAnywhere, Category="Info")
	FD1InteractionInfo InteractionInfo;
};
