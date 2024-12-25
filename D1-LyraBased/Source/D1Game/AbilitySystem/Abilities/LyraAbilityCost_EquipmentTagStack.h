#pragma once

#include "LyraAbilityCost.h"
#include "LyraAbilityCost_EquipmentTagStack.generated.h"

UCLASS(meta=(DisplayName="Equipment Tag Stack"))
class ULyraAbilityCost_EquipmentTagStack : public ULyraAbilityCost
{
	GENERATED_BODY()
	
public:
	ULyraAbilityCost_EquipmentTagStack();

protected:
	virtual bool CheckCost(const ULyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const ULyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AbilityCost")
	FScalableFloat Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AbilityCost")
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AbilityCost")
	FGameplayTag FailureTag;
};
