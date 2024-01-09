#include "D1InventoryItemDefinition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventoryItemDefinition)

UD1InventoryItemDefinition::UD1InventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

const UD1InventoryItemFragment* UD1InventoryItemDefinition::FindFragmentByClass(TSubclassOf<UD1InventoryItemFragment> FragmentClass) const
{
	if (FragmentClass)
	{
		for (UD1InventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}
	return nullptr;
}
