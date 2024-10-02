#include "D1ClassData.h"

#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ClassData)

const UD1ClassData& UD1ClassData::Get()
{
	return ULyraAssetManager::Get().GetClassData();
}

const FClassEntry& UD1ClassData::GetClassEntry(int32 ClassIndex) const
{
	if (ClassEntries.IsValidIndex(ClassIndex) == false)
	{
		static FClassEntry EmptyEntry;
		return EmptyEntry;
	}

	return ClassEntries[ClassIndex];
}
