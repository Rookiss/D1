#pragma once

#include "D1Define.h"
#include "D1ClassData.generated.h"

class UD1ItemTemplate;
class ULyraAbilitySet;

USTRUCT(BlueprintType)
struct FD1DefaultItemEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UD1ItemTemplate> ItemTemplateClass;

	UPROPERTY(EditDefaultsOnly)
	EItemRarity ItemRarity = EItemRarity::Poor;

	UPROPERTY(EditDefaultsOnly)
	int32 ItemCount = 1;
};

USTRUCT(BlueprintType)
struct FD1ClassInfoEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FText ClassName;

	UPROPERTY(EditDefaultsOnly)
	TArray<FD1DefaultItemEntry> DefaultItemEntries;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULyraAbilitySet> ClassAbilitySet;
};

UCLASS(BlueprintType, Const)
class UD1ClassData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	static const UD1ClassData& Get();

public:
	const FD1ClassInfoEntry& GetClassInfoEntry(ECharacterClassType ClassType) const;
	
private:
	UPROPERTY(EditDefaultsOnly, meta=(ArraySizeNum="ECharacterClassType"))
	FD1ClassInfoEntry ClassInfoEntries[(int32)ECharacterClassType::Count];
};
