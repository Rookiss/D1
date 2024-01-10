#include "InventoryItemFragment_InitialStats.h"

#include "Inventory/D1InventoryItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryItemFragment_InitialStats)

UInventoryItemFragment_InitialStats::UInventoryItemFragment_InitialStats(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UInventoryItemFragment_InitialStats::OnInstanceCreated(UD1InventoryItemInstance* Instance) const
{
	Super::OnInstanceCreated(Instance);
	
	for (const auto& Pair : InitialItemStats)
	{
		const FGameplayTag& StatTag = Pair.Key;
		const int32 StatCount = Pair.Value;
		Instance->AddStatTagStack(StatTag, StatCount);
	}
}

int32 UInventoryItemFragment_InitialStats::GetItemStatByTag(const FGameplayTag& StatTag) const
{
	if (const int32* StatPtr = InitialItemStats.Find(StatTag))
	{
		return *StatPtr;
	}
	return 0;
}
