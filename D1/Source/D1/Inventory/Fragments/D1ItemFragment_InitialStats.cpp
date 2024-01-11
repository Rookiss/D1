#include "D1ItemFragment_InitialStats.h"

#include "Inventory/D1ItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemFragment_InitialStats)

UD1ItemFragment_InitialStats::UD1ItemFragment_InitialStats(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1ItemFragment_InitialStats::OnInstanceCreated(UD1ItemInstance* Instance) const
{
	Super::OnInstanceCreated(Instance);
	
	for (const auto& Pair : InitialItemStats)
	{
		const FGameplayTag& StatTag = Pair.Key;
		const int32 StatCount = Pair.Value;
		Instance->AddStatTagStack(StatTag, StatCount);
	}
}

int32 UD1ItemFragment_InitialStats::GetItemStatByTag(const FGameplayTag& StatTag) const
{
	if (const int32* StatPtr = InitialItemStats.Find(StatTag))
	{
		return *StatPtr;
	}
	return 0;
}
