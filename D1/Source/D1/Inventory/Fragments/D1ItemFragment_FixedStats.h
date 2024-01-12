#pragma once

#include "GameplayTagContainer.h"
#include "D1ItemFragment.h"
#include "D1ItemFragment_FixedStats.generated.h"

UCLASS(BlueprintType, CollapseCategories)
class UD1ItemFragment_FixedStats : public UD1ItemFragment
{
	GENERATED_BODY()
	
public:
	UD1ItemFragment_FixedStats(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void OnInstanceCreated(UD1ItemInstance* Instance) const override;

public:
	int32 GetItemStatByTag(const FGameplayTag& StatTag) const;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ForceInlineRow))
	TMap<FGameplayTag, int32> FixedItemStats;
};
