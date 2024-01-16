#pragma once

#include "GameplayTags.h"
#include "D1ItemFragment.h"
#include "D1ItemFragment_RandomStats.generated.h"

USTRUCT(BlueprintType)
struct FStatRange
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MinValue = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxValue = 0;
};

UCLASS(BlueprintType, CollapseCategories)
class UD1ItemFragment_RandomStats : public UD1ItemFragment
{
	GENERATED_BODY()
	
public:
	UD1ItemFragment_RandomStats(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void OnInstanceCreated(UD1ItemInstance* Instance) const override;

public:
	const FStatRange GetItemStatByTag(const FGameplayTag& StatTag) const;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ForceInlineRow))
	TMap<FGameplayTag, FStatRange> RandomItemStats;
};
