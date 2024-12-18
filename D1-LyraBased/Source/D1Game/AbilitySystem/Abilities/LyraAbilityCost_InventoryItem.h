#pragma once

#include "LyraAbilityCost.h"
#include "LyraAbilityCost_InventoryItem.generated.h"

class UD1ItemTemplate;

UCLASS(meta=(DisplayName="Inventory Item"))
class ULyraAbilityCost_InventoryItem : public ULyraAbilityCost
{
	GENERATED_BODY()
	
public:
	ULyraAbilityCost_InventoryItem();

protected:
	virtual bool CheckCost(const ULyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const ULyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AbilityCost")
	FScalableFloat Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AbilityCost")
	TSubclassOf<UD1ItemTemplate> ItemTemplateClass;
};
