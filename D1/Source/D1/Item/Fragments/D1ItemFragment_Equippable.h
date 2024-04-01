#pragma once

#include "D1Define.h"
#include "GameplayTags.h"
#include "D1ItemFragment.h"
#include "D1ItemFragment_Equippable.generated.h"

class UD1ItemInstance;
class UD1AbilitySystemData;

USTRUCT()
struct FStatRange
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly)
	EItemRarity Rarity = EItemRarity::Junk;
	
	UPROPERTY(EditDefaultsOnly)
	int32 MinValue = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxValue = 0;
};

USTRUCT()
struct FStatRangeSet
{
	GENERATED_BODY()

public:
	FStatRangeSet();
	
public:
	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TArray<FStatRange> StatRanges;
};

UCLASS(Abstract, Const)
class UD1ItemFragment_Equippable : public UD1ItemFragment
{
	GENERATED_BODY()
	
public:
	UD1ItemFragment_Equippable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void OnInstanceCreated(UD1ItemInstance* ItemInstance) const override;

public:
	EEquipmentType EquipmentType = EEquipmentType::Count;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<const UD1AbilitySystemData> AbilitySystemData;
	
	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow))
	TMap<FGameplayTag, FStatRangeSet> Stats;
};
