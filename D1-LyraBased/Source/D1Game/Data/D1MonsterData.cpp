#include "D1MonsterData.h"

#include "AIController.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1MonsterData)

const UD1MonsterData& UD1MonsterData::Get()
{
	return ULyraAssetManager::Get().GetMonsterData();
}

ULyraPawnData* UD1MonsterData::GetPawnData(TSubclassOf<AAIController> AIControllerClass) const
{
	if (AIControllerClass == nullptr)
		return nullptr;
	
	const TObjectPtr<ULyraPawnData>* PawnData = PawnDataMap.Find(AIControllerClass);
	ensureAlwaysMsgf(PawnData, TEXT("Can't find Pawn Data from AI Controller Class [%s]"), *AIControllerClass->GetName());
	return PawnData ? PawnData->Get() : nullptr;
}
