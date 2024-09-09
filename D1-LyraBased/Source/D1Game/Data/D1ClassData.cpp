#include "D1ClassData.h"

#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ClassData)

const UD1ClassData& UD1ClassData::Get()
{
	return ULyraAssetManager::Get().GetClassData();
}
