#pragma once

#include "AbilitySystem/Abilities/Gladiator/D1GameplayAbility_Equipment.h"
#include "D1GameplayAbility_Skill_Buff.generated.h"

class UNiagaraSystem;

UCLASS()
class UD1GameplayAbility_Skill_Buff : public UD1GameplayAbility_Equipment
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Skill_Buff(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UFUNCTION()
	void ApplyEffect();
	
	UFUNCTION(BlueprintNativeEvent)
	void ApplyAdditionalEffects();

	UFUNCTION()
	void OnMontageFinished();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="D1|Buff")
	TObjectPtr<UAnimMontage> BuffMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="D1|Buff")
	TSubclassOf<UGameplayEffect> BuffGameplayEffectClass;

	UPROPERTY(EditDefaultsOnly, Category="D1|Buff")
	TObjectPtr<UNiagaraSystem> BuffEffect;
};
