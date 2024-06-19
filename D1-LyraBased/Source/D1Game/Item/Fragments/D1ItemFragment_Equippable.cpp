#include "D1ItemFragment_Equippable.h"

#include "Item/D1ItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemFragment_Equippable)

FRarityStatRangeSet::FRarityStatRangeSet()
{
	RarityStatRanges.SetNum((int32)EItemRarity::Count);
	for (int32 i = 0; i < RarityStatRanges.Num(); i++)
	{
		RarityStatRanges[i].Rarity = (EItemRarity)i;
	}
}

UD1ItemFragment_Equippable::UD1ItemFragment_Equippable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ItemFragment_Equippable::OnInstanceCreated(UD1ItemInstance* ItemInstance) const
{
	Super::OnInstanceCreated(ItemInstance);

	if (ItemInstance)
	{
		for (const auto& RarityStatRangeSet : RarityStatRangeSets)
		{
			const FGameplayTag& StatTag = RarityStatRangeSet.StatTag;
			const FRarityStatRange& StatRange = RarityStatRangeSet.RarityStatRanges[(int32)ItemInstance->GetItemRarity()];
			const int32 StatValue = FMath::RandRange(StatRange.MinValue, StatRange.MaxValue);
			ItemInstance->AddStatTagStack(StatTag, StatValue);
		}
	}
}
