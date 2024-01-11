#pragma once

#include "GameplayTagContainer.h"
#include "D1ItemFragment.h"
#include "D1ItemFragment_InitialStats.generated.h"

UCLASS(BlueprintType)
class UD1ItemFragment_InitialStats : public UD1ItemFragment
{
	GENERATED_BODY()
	
public:
	UD1ItemFragment_InitialStats(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void OnInstanceCreated(UD1ItemInstance* Instance) const override;

public:
	int32 GetItemStatByTag(const FGameplayTag& StatTag) const;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, int32> InitialItemStats;
};
