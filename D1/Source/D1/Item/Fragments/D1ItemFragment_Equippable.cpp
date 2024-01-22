#include "D1ItemFragment_Equippable.h"

#include "Item/D1ItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemFragment_Equippable)

FStatRangeSet::FStatRangeSet()
{
	for (int32 i = 0; i < (int32)EItemRarity::Count; i++)
	{
		StatRanges.FindOrAdd((EItemRarity)i);
	}
}

UD1ItemFragment_Equippable::UD1ItemFragment_Equippable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ItemFragment_Equippable::OnInstanceCreated(UD1ItemInstance* ItemInstance) const
{
	Super::OnInstanceCreated(ItemInstance);

	for (const auto& Pair : Stats)
	{
		// const FGameplayTag& StatTag = Pair.Key;
		// const FStatRange& StatRange = Pair.Value;
		// const int32 StatCount = FMath::RandRange(StatRange.MinValue, StatRange.MaxValue);
		// ItemInstance->AddStatTagStack(StatTag, StatCount);
	}
}
