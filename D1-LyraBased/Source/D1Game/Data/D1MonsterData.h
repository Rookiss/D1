#pragma once

#include "D1MonsterData.generated.h"

UCLASS(BlueprintType, Const, meta=(DisplayName="D1 Monster Data"))
class UD1MonsterData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	static const UD1MonsterData& Get();
};
