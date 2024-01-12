#include "D1ItemFragment_FixedStats.h"

#include "Inventory/D1ItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemFragment_FixedStats)

UD1ItemFragment_FixedStats::UD1ItemFragment_FixedStats(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1ItemFragment_FixedStats::OnInstanceCreated(UD1ItemInstance* Instance) const
{
	Super::OnInstanceCreated(Instance);
	
	for (const auto& Pair : FixedItemStats)
	{
		const FGameplayTag& StatTag = Pair.Key;
		const int32 StatCount = Pair.Value;
		Instance->AddStatTagStack(StatTag, StatCount);
	}
}

int32 UD1ItemFragment_FixedStats::GetItemStatByTag(const FGameplayTag& StatTag) const
{
	if (const int32* StatPtr = FixedItemStats.Find(StatTag))
	{
		return *StatPtr;
	}
	return 0;
}
