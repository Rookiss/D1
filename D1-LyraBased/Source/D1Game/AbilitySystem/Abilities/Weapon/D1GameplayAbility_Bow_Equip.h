#pragma once

#include "AbilitySystem/Abilities/Gladiator/D1GameplayAbility_Equipment.h"
#include "D1GameplayAbility_Bow_Equip.generated.h"

class UD1ItemTemplate;

UCLASS()
class UD1GameplayAbility_Bow_Equip : public UD1GameplayAbility_Equipment
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Bow_Equip(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UFUNCTION()
	void OnMontageFinished();

protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Bow Equip")
	TObjectPtr<UAnimMontage> EquipMontage_Empty;

	UPROPERTY(EditDefaultsOnly, Category="D1|Bow Equip")
	TObjectPtr<UAnimMontage> EquipMontage_Arrow;

	UPROPERTY(EditDefaultsOnly, Category="D1|Bow Equip")
	TSubclassOf<UD1ItemTemplate> ArrowItemTemplateClass;
};
