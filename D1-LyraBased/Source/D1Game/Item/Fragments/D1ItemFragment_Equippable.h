#pragma once

#include "D1Define.h"
#include "GameplayTags.h"
#include "Item/D1ItemTemplate.h"
#include "D1ItemFragment_Equippable.generated.h"

class ULyraAbilitySet;
class UD1ItemInstance;

USTRUCT()
struct FRarityStatRange
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
struct FRarityStatRangeSet
{
	GENERATED_BODY()

public:
	FRarityStatRangeSet();
	
public:
	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TArray<FRarityStatRange> RarityStatRanges;
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
	TObjectPtr<const ULyraAbilitySet> BaseAbilitySet;
	
	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow))
	TMap<FGameplayTag, FRarityStatRangeSet> BaseStats;
};
