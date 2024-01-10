#include "D1InventoryItemDefinition.h"

#include "Fragments/InventoryItemFragment.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventoryItemDefinition)

UD1InventoryItemDefinition::UD1InventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

const UInventoryItemFragment* UD1InventoryItemDefinition::FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass)
	{
		for (UInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}
	return nullptr;
}
