#pragma once

#include "D1InteractionInfo.h"
#include "UObject/Interface.h"
#include "D1Interactable.generated.h"

struct FD1InteractionInfo;

UINTERFACE(MinimalAPI, BlueprintType)
class UD1Interactable : public UInterface
{
	GENERATED_BODY()
};

class ID1Interactable
{
	GENERATED_BODY()

public:
	virtual const FD1InteractionInfo& GetInteractionInfo() const abstract;
};
