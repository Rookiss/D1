#pragma once

#include "D1Define.h"
#include "D1ClassData.generated.h"

class UD1ItemTemplate;
class ULyraAbilitySet;
class UTexture2D;
class ULyraGameplayAbility;

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
	ECharacterClassType ClassType = ECharacterClassType::Count;
	
	UPROPERTY(EditDefaultsOnly)
	FText ClassName;

	UPROPERTY(EditDefaultsOnly)
	TArray<FD1DefaultItemEntry> DefaultItemEntries;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULyraAbilitySet> ClassAbilitySet;
};

USTRUCT(BlueprintType)
struct FD1ClassInfoSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FD1ClassInfoEntry> ClassInfoEntries;
};

UCLASS(BlueprintType, Const)
class UD1ClassData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	static const UD1ClassData& Get();

protected:
#if WITH_EDITOR
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

public:
	const FD1ClassInfoEntry& GetClassEntry(ECharacterClassType ClassType) const;
	const TArray<FD1ClassInfoEntry>& GetClassEntries() const { return ClassInfoSet.ClassInfoEntries; }
	
private:
	UPROPERTY(EditDefaultsOnly)
	FD1ClassInfoSet ClassInfoSet;
};
