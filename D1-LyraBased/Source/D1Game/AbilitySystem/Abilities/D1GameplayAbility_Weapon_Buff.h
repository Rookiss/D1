#pragma once

#include "D1GameplayAbility_Weapon.h"
#include "D1GameplayAbility_Weapon_Buff.generated.h"

class UNiagaraSystem;

UCLASS()
class UD1GameplayAbility_Weapon_Buff : public UD1GameplayAbility_Weapon
{
	GENERATED_BODY()
	
public:
	UD1GameplayAbility_Weapon_Buff(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

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
