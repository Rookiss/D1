#include "D1GameplayAbility_Weapon_Buff.h"

#include "NiagaraSystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_Buff)

UD1GameplayAbility_Weapon_Buff::UD1GameplayAbility_Weapon_Buff(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Weapon_Buff::ApplyEffect()
{
	if (BuffGameplayEffectClass == nullptr)
		return;

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (SourceASC == nullptr)
		return;
	
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(BuffGameplayEffectClass);
	
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(BuffEffect);
	EffectSpecHandle.Data->SetContext(EffectContextHandle);
	
	ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle);
}

void UD1GameplayAbility_Weapon_Buff::AdditionalEffects_Implementation()
{
	
}
