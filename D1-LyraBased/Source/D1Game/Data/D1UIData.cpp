#include "D1UIData.h"

#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1UIData)

const UD1UIData& UD1UIData::Get()
{
	return ULyraAssetManager::Get().GetUIData();
}

UTexture2D* UD1UIData::GetEntryRarityTexture(EItemRarity ItemRarity) const
{
	const int32 ItemRarityIndex = (int32)ItemRarity;
	if (ItemRarityIndex < 0 || ItemRarityIndex >= (int32)EItemRarity::Count)
		return nullptr;
	
	return RarityInfoEntries[ItemRarityIndex].EntryTexture;
}

UTexture2D* UD1UIData::GetHoverRarityTexture(EItemRarity ItemRarity) const
{
	const int32 ItemRarityIndex = (int32)ItemRarity;
	if (ItemRarityIndex < 0 || ItemRarityIndex >= (int32)EItemRarity::Count)
		return nullptr;
	
	return RarityInfoEntries[ItemRarityIndex].HoverTexture;
}

FColor UD1UIData::GetRarityColor(EItemRarity ItemRarity) const
{
	const int32 ItemRarityIndex = (int32)ItemRarity;
	if (ItemRarityIndex < 0 || ItemRarityIndex >= (int32)EItemRarity::Count)
		return FColor::Black;
	
	return RarityInfoEntries[ItemRarityIndex].Color;
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
