#pragma once

#include "D1Interactable.h"
#include "D1InteractionInfo.h"
#include "D1WorldInteractable.generated.h"

UCLASS(Abstract, Blueprintable)
class AD1WorldInteractable : public AActor, public ID1Interactable
{
	GENERATED_BODY()
	
public:
	AD1WorldInteractable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void AddInteractionInfo(TArray<FD1InteractionInfo>& OutInteractionInfos);
	
protected:
	UPROPERTY(EditAnywhere)
	FD1InteractionInfo InteractionInfo;
};
