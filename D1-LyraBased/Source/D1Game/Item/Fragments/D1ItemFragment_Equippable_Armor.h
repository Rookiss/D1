#pragma once

#include "D1Define.h"
#include "D1ItemFragment_Equippable.h"
#include "D1ItemFragment_Equippable_Armor.generated.h"

UCLASS()
class UD1ItemFragment_Equippable_Armor : public UD1ItemFragment_Equippable
{
	GENERATED_BODY()
	
public:
	UD1ItemFragment_Equippable_Armor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA
	
public:
	UPROPERTY(EditDefaultsOnly)
	EArmorType ArmorType = EArmorType::Count;

	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="ArmorType == EArmorType::Chest", EditConditionHides))
	bool bIsFullBody = false;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<USkeletalMesh> ArmorMesh;
};
