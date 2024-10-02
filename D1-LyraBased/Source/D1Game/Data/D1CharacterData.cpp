#include "D1CharacterData.h"

#include "System/LyraAssetManager.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1CharacterData)

FD1DefaultArmorMeshSet::FD1DefaultArmorMeshSet()
{
	const int32 ArmorTypeCount = (int32)EArmorType::Count;

	DefaultMeshEntries.SetNum(ArmorTypeCount);
	for (int32 i = 0; i < ArmorTypeCount; i++)
	{
		DefaultMeshEntries[i].ArmorType = (EArmorType)i;
	}

	SecondaryMeshEntries.SetNum(ArmorTypeCount);
	for (int32 i = 0; i < ArmorTypeCount; i++)
	{
		SecondaryMeshEntries[i].ArmorType = (EArmorType)i;
	}
}

const UD1CharacterData& UD1CharacterData::Get()
{
	return ULyraAssetManager::Get().GetCharacterData();
}

void UD1CharacterData::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);

	DefaultArmorMeshMap.KeySort([](const ECharacterSkinType A, const ECharacterSkinType B)
	{
		return (A < B);
	});
}

const FD1DefaultArmorMeshSet& UD1CharacterData::GetDefaultArmorMeshSet(ECharacterSkinType CharacterSkinType) const
{
	if (DefaultArmorMeshMap.Contains(CharacterSkinType) == false)
	{
		static FD1DefaultArmorMeshSet EmptyEntry;
		return EmptyEntry;
	}

	return DefaultArmorMeshMap.FindChecked(CharacterSkinType);
}
