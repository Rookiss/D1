#include "D1CheatData.h"

#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1CheatData)

const UD1CheatData& UD1CheatData::Get()
{
	return ULyraAssetManager::Get().GetCheatData();
}
