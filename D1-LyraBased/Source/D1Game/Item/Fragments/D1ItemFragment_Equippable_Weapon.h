#pragma once

#include "D1Define.h"
#include "D1ItemFragment_Equippable_Attachment.h"
#include "D1ItemFragment_Equippable_Weapon.generated.h"

UCLASS()
class UD1ItemFragment_Equippable_Weapon : public UD1ItemFragment_Equippable_Attachment
{
	GENERATED_BODY()
	
public:
	UD1ItemFragment_Equippable_Weapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void OnInstanceCreated(UD1ItemInstance* ItemInstance) const override;
	
public:
	UPROPERTY(EditDefaultsOnly)
	EWeaponType WeaponType = EWeaponType::Count;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<const ULyraAbilitySet>> SkillAbilitySets;
	
	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow))
	TArray<FRarityStatRangeSet> RarityStatRangeSets;
};
