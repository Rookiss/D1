#pragma once

#include "D1Define.h"
#include "D1ItemFragment_Equipable.h"
#include "D1ItemFragment_Equipable_Armor.generated.h"

UCLASS()
class UD1ItemFragment_Equipable_Armor : public UD1ItemFragment_Equipable
{
	GENERATED_BODY()
	
public:
	UD1ItemFragment_Equipable_Armor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA

public:
	virtual void OnInstanceCreated(UD1ItemInstance* ItemInstance) const override;
	
public:
	UPROPERTY(EditDefaultsOnly)
	EArmorType ArmorType = EArmorType::Count;

	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="ArmorType == EArmorType::Chest", EditConditionHides))
	bool bIsFullBody = false;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<USkeletalMesh> ArmorMesh;

	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow))
	TArray<FRarityStatRangeSet> RarityStatRangeSets;
};
