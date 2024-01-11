#pragma once

#include "D1ItemFragment.generated.h"

class UD1ItemInstance;

UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class UD1ItemFragment : public UObject
{
	GENERATED_BODY()

public:
	UD1ItemFragment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
public:
	virtual void OnInstanceCreated(UD1ItemInstance* Instance) const { }
};
