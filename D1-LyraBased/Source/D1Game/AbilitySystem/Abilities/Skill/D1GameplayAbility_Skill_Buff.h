#pragma once

#include "AbilitySystem/Abilities/Weapon/D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_Skill_Buff.generated.h"

class UNiagaraSystem;

UCLASS()
class UD1GameplayAbility_Skill_Buff : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Skill_Buff(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UFUNCTION(BlueprintCallable)
	void ApplyEffect();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AdditionalEffects();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> BuffGameplayEffectClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> BuffEffect;
};
