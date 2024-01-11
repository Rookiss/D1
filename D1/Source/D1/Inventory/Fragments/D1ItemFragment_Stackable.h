#pragma once

#include "D1ItemFragment.h"
#include "D1ItemFragment_Stackable.generated.h"

UCLASS(BlueprintType)
class UD1ItemFragment_Stackable : public UD1ItemFragment
{
	GENERATED_BODY()
	
public:
	UD1ItemFragment_Stackable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
