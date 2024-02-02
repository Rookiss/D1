#pragma once

#include "D1ItemFragment.h"
#include "Item/D1ItemInstance.h"
#include "D1ItemFragment_Equippable.generated.h"

class UD1AbilitySystemData;
class UD1EquipmentInstance;

UENUM()
enum class EEquipmentType
{
	Weapon,
	Armor,
	
	Count	UMETA(Hidden)
};

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
	TObjectPtr<const UD1AbilitySystemData> AbilitySystemDataToGrant;
	
	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow))
	TMap<FGameplayTag, FStatRangeSet> Stats;
};
