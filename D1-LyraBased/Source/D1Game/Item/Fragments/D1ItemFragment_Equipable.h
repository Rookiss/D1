#pragma once

#include "D1Define.h"
#include "GameplayTags.h"
#include "AbilitySystem/D1AbilitySourceInterface.h"
#include "Item/D1ItemTemplate.h"
#include "D1ItemFragment_Equipable.generated.h"

class ULyraAbilitySet;
class UD1ItemInstance;

USTRUCT(BlueprintType)
struct FRarityStat
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly)
	EItemRarity Rarity = EItemRarity::Poor;

	UPROPERTY(EditDefaultsOnly)
	int32 Value = 0;
};

USTRUCT(BlueprintType)
struct FRarityStatSet
{
	GENERATED_BODY()

public:
	FRarityStatSet();
	
public:
	UPROPERTY(EditDefaultsOnly, meta=(Categories="SetByCaller"))
	FGameplayTag StatTag;
	
	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TArray<FRarityStat> RarityStats;
};

USTRUCT(BlueprintType)
struct FRarityStatRange
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly)
	EItemRarity Rarity = EItemRarity::Poor;
	
	UPROPERTY(EditDefaultsOnly)
	int32 MinValue = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxValue = 0;
};

USTRUCT(BlueprintType)
struct FRarityStatRangeSet
{
	GENERATED_BODY()

public:
	FRarityStatRangeSet();
	
public:
	UPROPERTY(EditDefaultsOnly, meta=(Categories="SetByCaller"))
	FGameplayTag StatTag;
	
	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TArray<FRarityStatRange> RarityStatRanges;
};

UCLASS(Abstract, Const)
class UD1ItemFragment_Equipable : public UD1ItemFragment, public ID1AbilitySourceInterface
{
	GENERATED_BODY()
	
public:
	UD1ItemFragment_Equipable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	void AddStatTagStack(UD1ItemInstance* ItemInstance, const TArray<FRarityStatSet>& RarityStatSets) const;
	void AddStatTagStack(UD1ItemInstance* ItemInstance, const TArray<FRarityStatRangeSet>& RarityStatRangeSets) const;

public:
	bool IsEquipableClassType(ECharacterClassType ClassType) const;
	virtual float GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const;
	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const override;
	
public:
	EEquipmentType EquipmentType = EEquipmentType::Count;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Equipable", meta=(Bitmask, BitmaskEnum="ECharacterClassType"))
	uint32 EquipableClassFlags = ((1 << (uint32)ECharacterClassType::Count) - 1);
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Equipable")
	TObjectPtr<const ULyraAbilitySet> BaseAbilitySet;

	UPROPERTY(EditDefaultsOnly, Category="D1|Equipable", meta=(ForceInlineRow))
	TMap<FGameplayTag, float> MaterialTagMultiplier;
};
