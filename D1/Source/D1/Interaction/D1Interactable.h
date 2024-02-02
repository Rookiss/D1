#pragma once

#include "UObject/Interface.h"
#include "D1Interactable.generated.h"

struct FD1InteractionInfo;

UINTERFACE(MinimalAPI)
class UD1Interactable : public UInterface
{
	GENERATED_BODY()
};

class ID1Interactable
{
	GENERATED_BODY()

public:
	virtual void AddInteractionInfo(TArray<FD1InteractionInfo>& OutInteractionInfos) abstract;
};
