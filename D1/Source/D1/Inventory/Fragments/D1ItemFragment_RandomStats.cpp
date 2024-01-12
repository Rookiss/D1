#include "D1ItemFragment_RandomStats.h"

#include "Inventory/D1ItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemFragment_RandomStats)

UD1ItemFragment_RandomStats::UD1ItemFragment_RandomStats(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ItemFragment_RandomStats::OnInstanceCreated(UD1ItemInstance* Instance) const
{
	Super::OnInstanceCreated(Instance);

	for (const auto& Pair : RandomItemStats)
	{
		const FGameplayTag& StatTag = Pair.Key;
		const FStatRange& StatRange = Pair.Value;
		const int32 StatCount = FMath::RandRange(StatRange.MinValue, StatRange.MaxValue);
		Instance->AddStatTagStack(StatTag, StatCount);
	}
}

const FStatRange UD1ItemFragment_RandomStats::GetItemStatByTag(const FGameplayTag& StatTag) const
{
	if (const FStatRange* StatPtr = RandomItemStats.Find(StatTag))
	{
		return *StatPtr;
	}
	return FStatRange();
}
