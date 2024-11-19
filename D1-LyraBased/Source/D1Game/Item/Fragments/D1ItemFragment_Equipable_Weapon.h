#pragma once

#include "D1Define.h"
#include "D1ItemFragment_Equipable_Attachment.h"
#include "D1ItemFragment_Equipable_Weapon.generated.h"

UCLASS()
class UD1ItemFragment_Equipable_Weapon : public UD1ItemFragment_Equipable_Attachment
{
	GENERATED_BODY()
	
public:
	UD1ItemFragment_Equipable_Weapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA
	
public:
	virtual void OnInstanceCreated(UD1ItemInstance* ItemInstance) const override;
	
public:
	UPROPERTY(EditDefaultsOnly)
	EWeaponType WeaponType = EWeaponType::Count;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> AttackSwingSound;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<const ULyraAbilitySet>> SkillAbilitySets;
	
	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow))
	TArray<FRarityStatRangeSet> RarityStatRangeSets;
};
