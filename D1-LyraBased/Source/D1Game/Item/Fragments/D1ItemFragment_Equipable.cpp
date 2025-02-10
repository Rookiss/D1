#include "D1ItemFragment_Equipable.h"

#include "Item/D1ItemInstance.h"
#include "Physics/PhysicalMaterialWithTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemFragment_Equipable)

FRarityStatSet::FRarityStatSet()
{
	RarityStats.SetNum((int32)EItemRarity::Count);
	for (int32 i = 0; i < RarityStats.Num(); i++)
	{
		RarityStats[i].Rarity = (EItemRarity)i;
	}
}

FRarityStatRangeSet::FRarityStatRangeSet()
{
	RarityStatRanges.SetNum((int32)EItemRarity::Count);
	for (int32 i = 0; i < RarityStatRanges.Num(); i++)
	{
		RarityStatRanges[i].Rarity = (EItemRarity)i;
	}
}

UD1ItemFragment_Equipable::UD1ItemFragment_Equipable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ItemFragment_Equipable::AddStatTagStack(UD1ItemInstance* ItemInstance, const TArray<FRarityStatSet>& RarityStatSets) const
{
	if (ItemInstance == nullptr)
		return;

	for (const FRarityStatSet& RarityStatSet : RarityStatSets)
	{
		const FGameplayTag& StatTag = RarityStatSet.StatTag;
		const FRarityStat& Stat = RarityStatSet.RarityStats[(int32)ItemInstance->GetItemRarity()];
		ItemInstance->AddOrRemoveStatTagStack(StatTag, Stat.Value);
	}
}

void UD1ItemFragment_Equipable::AddStatTagStack(UD1ItemInstance* ItemInstance, const TArray<FRarityStatRangeSet>& RarityStatRangeSets) const
{
	if (ItemInstance == nullptr)
		return;
	
	for (const FRarityStatRangeSet& RarityStatRangeSet : RarityStatRangeSets)
	{
		const FGameplayTag& StatTag = RarityStatRangeSet.StatTag;
		const FRarityStatRange& StatRange = RarityStatRangeSet.RarityStatRanges[(int32)ItemInstance->GetItemRarity()];
		const int32 StatValue = FMath::RandRange(StatRange.MinValue, StatRange.MaxValue);
		ItemInstance->AddOrRemoveStatTagStack(StatTag, StatValue);
	}
}

bool UD1ItemFragment_Equipable::IsEquipableClassType(ECharacterClassType ClassType) const
{
	return (EquipableClassFlags & (1 << (uint32)ClassType)) > 0;
}

float UD1ItemFragment_Equipable::GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags) const
{
	return 1.f;
}

float UD1ItemFragment_Equipable::GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags) const
{
	float CombinedMultiplier = 1.f;
	
	if (const UPhysicalMaterialWithTags* PhysMatWithTags = Cast<const UPhysicalMaterialWithTags>(PhysicalMaterial))
	{
		for (const FGameplayTag MaterialTag : PhysMatWithTags->Tags)
		{
			if (const float* Multiplier = MaterialTagMultiplier.Find(MaterialTag))
			{
				CombinedMultiplier *= *Multiplier;
			}
		}
	}

	return CombinedMultiplier;
}
