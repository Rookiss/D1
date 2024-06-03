#include "D1ItemTemplate.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemTemplate)

UD1ItemTemplate::UD1ItemTemplate(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

const UD1ItemFragment* UD1ItemTemplate::FindFragmentByClass(TSubclassOf<UD1ItemFragment> FragmentClass) const
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
