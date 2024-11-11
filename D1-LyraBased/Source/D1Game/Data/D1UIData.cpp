#include "D1UIData.h"

#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1UIData)

FD1ItemRarityInfoSet::FD1ItemRarityInfoSet()
{
	const int32 ItemRarityCount = (int32)EItemRarity::Count;
	
	RarityInfoEntries.SetNum(ItemRarityCount);
	for (int32 i = 0; i < ItemRarityCount; i++)
	{
		RarityInfoEntries[i].Rarity = (EItemRarity)i;
	}
}

UTexture2D* FD1ItemRarityInfoSet::GetEntryTexture(EItemRarity ItemRarity) const
{
	const int32 ItemRarityIndex = (int32)ItemRarity;
	if (RarityInfoEntries.IsValidIndex(ItemRarityIndex))
	{
		return RarityInfoEntries[ItemRarityIndex].EntryTexture;
	}
	return nullptr;
}

UTexture2D* FD1ItemRarityInfoSet::GetHoverTexture(EItemRarity ItemRarity) const
{
	const int32 ItemRarityIndex = (int32)ItemRarity;
	if (RarityInfoEntries.IsValidIndex(ItemRarityIndex))
	{
		return RarityInfoEntries[ItemRarityIndex].HoverTexture;
	}
	return nullptr;
}

FColor FD1ItemRarityInfoSet::GetRarityColor(EItemRarity ItemRarity) const
{
	const int32 ItemRarityIndex = (int32)ItemRarity;
	if (RarityInfoEntries.IsValidIndex(ItemRarityIndex))
	{
		return RarityInfoEntries[ItemRarityIndex].Color;
	}
	return FColor::Black;
}

const UD1UIData& UD1UIData::Get()
{
	return ULyraAssetManager::Get().GetUIData();
}

UTexture2D* UD1UIData::GetEntryRarityTexture(EItemRarity ItemRarity) const
{
	return RarityInfoSet.GetEntryTexture(ItemRarity);
}

UTexture2D* UD1UIData::GetHoverRarityTexture(EItemRarity ItemRarity) const
{
	return RarityInfoSet.GetHoverTexture(ItemRarity);
}

FColor UD1UIData::GetRarityColor(EItemRarity ItemRarity) const
{
	return RarityInfoSet.GetRarityColor(ItemRarity);
}

const FD1UIInfo& UD1UIData::GetTagUIInfo(FGameplayTag Tag) const
{
	const FD1UIInfo* UIInfo = TagUIInfos.Find(Tag);
	if (UIInfo == nullptr)
	{
		static FD1UIInfo EmptyInfo;
		return EmptyInfo;
	}

	return *UIInfo;
}
