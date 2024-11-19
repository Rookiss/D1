#include "D1ClassData.h"

#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ClassData)

const UD1ClassData& UD1ClassData::Get()
{
	return ULyraAssetManager::Get().GetClassData();
}

const FD1ClassInfoEntry& UD1ClassData::GetClassInfoEntry(ECharacterClassType ClassType) const
{
	const int32 ClassIndex = (int32)ClassType;
	if (ClassIndex < 0 || ClassIndex >= (int32)ECharacterClassType::Count)
	{
		static FD1ClassInfoEntry EmptyEntry;
		return EmptyEntry;
	}
	
	return ClassInfoEntries[ClassIndex];
}
