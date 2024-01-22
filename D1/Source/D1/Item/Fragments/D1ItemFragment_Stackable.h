#pragma once

#include "D1ItemFragment.h"
#include "D1ItemFragment_Stackable.generated.h"

UCLASS(CollapseCategories)
class UD1ItemFragment_Stackable : public UD1ItemFragment
{
	GENERATED_BODY()
	
public:
	UD1ItemFragment_Stackable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(EditDefaultsOnly)
	int32 MaxStackCount = 2;
};
