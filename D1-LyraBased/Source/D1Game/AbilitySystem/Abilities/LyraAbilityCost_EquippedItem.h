#pragma once

#include "D1Define.h"
#include "LyraAbilityCost.h"
#include "ScalableFloat.h"
#include "Templates/SubclassOf.h"
#include "LyraAbilityCost_EquippedItem.generated.h"

class UD1ItemTemplate;
class ULyraGameplayAbility;
struct FGameplayAbilityActivationInfo;
struct FGameplayAbilitySpecHandle;
struct FGameplayAbilityActorInfo;
struct FGameplayTagContainer;

UCLASS(meta=(DisplayName="Equipped Item"))
class ULyraAbilityCost_EquippedItem : public ULyraAbilityCost
{
	GENERATED_BODY()

public:
	ULyraAbilityCost_EquippedItem();

protected:
	virtual bool CheckCost(const ULyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const ULyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AbilityCost)
	FScalableFloat Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AbilityCost)
	EEquipmentType EquipmentType = EEquipmentType::Count;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AbilityCost, meta=(EditCondition="EquipmentType == EEquipmentType::Weapon", EditConditionHides))
	EWeaponHandType WeaponHandType = EWeaponHandType::Count;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AbilityCost, meta=(EditCondition="EquipmentType == EEquipmentType::Armor", EditConditionHides))
	EArmorType ArmorType = EArmorType::Count;
};
