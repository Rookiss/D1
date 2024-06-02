#include "D1ItemDefinition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemDefinition)

UD1ItemDefinition::UD1ItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

const UD1ItemFragment* UD1ItemDefinition::FindFragmentByClass(TSubclassOf<UD1ItemFragment> FragmentClass) const
{
	if (FragmentClass)
	{
		for (UD1ItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}
	return nullptr;
}
