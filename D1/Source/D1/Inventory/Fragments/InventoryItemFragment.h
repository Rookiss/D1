#pragma once

#include "InventoryItemFragment.generated.h"

class UD1InventoryItemInstance;

UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class UInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	UInventoryItemFragment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
public:
	virtual void OnInstanceCreated(UD1InventoryItemInstance* Instance) const { }
};
