#pragma once

#include "D1MonsterData.generated.h"

class ULyraPawnData;
class AAIController;

UCLASS(BlueprintType, Const, meta=(DisplayName="D1 Monster Data"))
class UD1MonsterData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	static const UD1MonsterData& Get();

public:
	ULyraPawnData* GetPawnData(TSubclassOf<AAIController> AIControllerClass) const;
	
public:
	UPROPERTY(EditDefaultsOnly)
	TMap<TSubclassOf<AAIController>, TObjectPtr<ULyraPawnData>> PawnDataMap;
};
