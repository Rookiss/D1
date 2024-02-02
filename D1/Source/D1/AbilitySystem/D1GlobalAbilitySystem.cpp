#include "D1GlobalAbilitySystem.h"

#include "D1AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GlobalAbilitySystem)

void FGlobalAppliedAbilityList::AddToASC(TSubclassOf<UGameplayAbility> Ability, UD1AbilitySystemComponent* ASC)
{
	if (Handles.Find(ASC))
	{
		RemoveFromASC(ASC);
	}

	UGameplayAbility* AbilityCDO = Ability->GetDefaultObject<UGameplayAbility>();
	FGameplayAbilitySpec AbilitySpec(AbilityCDO);
	const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
	Handles.Add(ASC, AbilitySpecHandle);
}

void FGlobalAppliedAbilityList::RemoveFromASC(UD1AbilitySystemComponent* ASC)
{
	if (FGameplayAbilitySpecHandle* SpecHandle = Handles.Find(ASC))
	{
		ASC->ClearAbility(*SpecHandle);
		Handles.Remove(ASC);
	}
}

void FGlobalAppliedAbilityList::RemoveFromAll()
{
	for (const auto& Handle : Handles)
	{
		if (Handle.Key)
		{
			Handle.Key->ClearAbility(Handle.Value);
		}
	}
	Handles.Empty();
}

void FGlobalAppliedEffectList::AddToASC(TSubclassOf<UGameplayEffect> Effect, UD1AbilitySystemComponent* ASC)
{
	if (Handles.Find(ASC))
	{
		RemoveFromASC(ASC);
	}

	const UGameplayEffect* GameplayEffectCDO = Effect->GetDefaultObject<UGameplayEffect>();
	const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffectCDO, 1, ASC->MakeEffectContext());
	Handles.Add(ASC, GameplayEffectHandle);
}

void FGlobalAppliedEffectList::RemoveFromASC(UD1AbilitySystemComponent* ASC)
{
	if (FActiveGameplayEffectHandle* EffectHandle = Handles.Find(ASC))
	{
		ASC->RemoveActiveGameplayEffect(*EffectHandle);
		Handles.Remove(ASC);
	}
}

void FGlobalAppliedEffectList::RemoveFromAll()
{
	for (const auto& Handle : Handles)
	{
		if (Handle.Key)
		{
			Handle.Key->RemoveActiveGameplayEffect(Handle.Value);
		}
	}
	Handles.Empty();
}

UD1GlobalAbilitySystem::UD1GlobalAbilitySystem()
	: Super()
{
    
}

void UD1GlobalAbilitySystem::RegisterASC(UD1AbilitySystemComponent* ASC)
{
	check(ASC);

	for (auto& AppliedAbility : AppliedAbilities)
	{
		AppliedAbility.Value.AddToASC(AppliedAbility.Key, ASC);
	}
	for (auto& AppliedEffect : AppliedEffects)
	{
		AppliedEffect.Value.AddToASC(AppliedEffect.Key, ASC);
	}

	RegisteredASCs.AddUnique(ASC);
}

void UD1GlobalAbilitySystem::UnregisterASC(UD1AbilitySystemComponent* ASC)
{
	check(ASC);
	
	for (auto& AppliedAbility : AppliedAbilities)
	{
		AppliedAbility.Value.RemoveFromASC(ASC);
	}
	for (auto& AppliedEffect : AppliedEffects)
	{
		AppliedEffect.Value.RemoveFromASC(ASC);
	}

	RegisteredASCs.Remove(ASC);
}

void UD1GlobalAbilitySystem::ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability)
{
	if (Ability.Get() && AppliedAbilities.Contains(Ability) == false)
	{
		FGlobalAppliedAbilityList& AppliedAbility = AppliedAbilities.Add(Ability);		
		for (UD1AbilitySystemComponent* ASC : RegisteredASCs)
		{
			AppliedAbility.AddToASC(Ability, ASC);
		}
	}
}

void UD1GlobalAbilitySystem::ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect)
{
	if (Effect.Get() && AppliedEffects.Contains(Effect) == false)
	{
		FGlobalAppliedEffectList& AppliedEffect = AppliedEffects.Add(Effect);
		for (UD1AbilitySystemComponent* ASC : RegisteredASCs)
		{
			AppliedEffect.AddToASC(Effect, ASC);
		}
	}
}

void UD1GlobalAbilitySystem::RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability)
{
	if (Ability.Get() && AppliedAbilities.Contains(Ability))
	{
		FGlobalAppliedAbilityList& AppliedAbility = AppliedAbilities[Ability];
		AppliedAbility.RemoveFromAll();
		AppliedAbilities.Remove(Ability);
	}
}

void UD1GlobalAbilitySystem::RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect)
{
	if (Effect.Get() && AppliedEffects.Contains(Effect))
	{
		FGlobalAppliedEffectList& AppliedEffect = AppliedEffects[Effect];
		AppliedEffect.RemoveFromAll();
		AppliedEffects.Remove(Effect);
	}
}
