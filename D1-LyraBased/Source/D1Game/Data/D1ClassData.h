#pragma once

#include "D1Define.h"
#include "D1ClassData.generated.h"

class UD1ItemTemplate;
class ULyraAbilitySet;
class UTexture2D;
class ULyraGameplayAbility;

USTRUCT()
struct FDefaultItemEntry
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

USTRUCT()
struct FClassEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FText ClassName;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> ClassIcon;

	UPROPERTY(EditDefaultsOnly)
	TArray<FDefaultItemEntry> DefaultItemEntries;
	
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
	const TArray<FClassEntry>& GetClassEntries() const { return ClassEntries; }
	const FClassEntry& GetClassEntry(int32 ClassIndex) const;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TArray<FClassEntry> ClassEntries;
};
