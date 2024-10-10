#include "D1MonsterData.h"

#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1MonsterData)

const UD1MonsterData& UD1MonsterData::Get()
{
	return ULyraAssetManager::Get().GetMonsterData();
}
