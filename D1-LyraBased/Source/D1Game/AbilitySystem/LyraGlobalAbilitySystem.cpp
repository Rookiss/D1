// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraGlobalAbilitySystem.h"

#include "AbilitySystem/LyraAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraGlobalAbilitySystem)

void FGlobalAppliedAbilityList::AddToASC(TSubclassOf<UGameplayAbility> Ability, ULyraAbilitySystemComponent* ASC)
{
	if (FGameplayAbilitySpecHandle* SpecHandle = Handles.Find(ASC))
	{
		RemoveFromASC(ASC);
	}

	UGameplayAbility* AbilityCDO = Ability->GetDefaultObject<UGameplayAbility>();
	FGameplayAbilitySpec AbilitySpec(AbilityCDO);
	const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
	Handles.Add(ASC, AbilitySpecHandle);
}

void FGlobalAppliedAbilityList::RemoveFromASC(ULyraAbilitySystemComponent* ASC)
{
	if (FGameplayAbilitySpecHandle* SpecHandle = Handles.Find(ASC))
	{
		ASC->ClearAbility(*SpecHandle);
		Handles.Remove(ASC);
	}
}

void FGlobalAppliedAbilityList::RemoveFromAll()
{
	for (auto& KVP : Handles)
	{
		if (KVP.Key != nullptr)
		{
			KVP.Key->ClearAbility(KVP.Value);
		}
	}
	Handles.Empty();
}



void FGlobalAppliedEffectList::AddToASC(TSubclassOf<UGameplayEffect> Effect, ULyraAbilitySystemComponent* ASC)
{
	if (FActiveGameplayEffectHandle* EffectHandle = Handles.Find(ASC))
	{
		RemoveFromASC(ASC);
	}

	const UGameplayEffect* GameplayEffectCDO = Effect->GetDefaultObject<UGameplayEffect>();
	const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffectCDO, /*Level=*/ 1, ASC->MakeEffectContext());
	Handles.Add(ASC, GameplayEffectHandle);
}

void FGlobalAppliedEffectList::RemoveFromASC(ULyraAbilitySystemComponent* ASC)
{
	if (FActiveGameplayEffectHandle* EffectHandle = Handles.Find(ASC))
	{
		ASC->RemoveActiveGameplayEffect(*EffectHandle);
		Handles.Remove(ASC);
	}
}

void FGlobalAppliedEffectList::RemoveFromAll()
{
	for (auto& KVP : Handles)
	{
		if (KVP.Key != nullptr)
		{
			KVP.Key->RemoveActiveGameplayEffect(KVP.Value);
		}
	}
	Handles.Empty();
}

ULyraGlobalAbilitySystem::ULyraGlobalAbilitySystem()
{
}

void ULyraGlobalAbilitySystem::ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability)
{
	if ((Ability.Get() != nullptr) && (!AppliedAbilities.Contains(Ability)))
	{
		FGlobalAppliedAbilityList& Entry = AppliedAbilities.Add(Ability);		
		for (ULyraAbilitySystemComponent* ASC : RegisteredASCs)
		{
			Entry.AddToASC(Ability, ASC);
		}
	}
}

void ULyraGlobalAbilitySystem::ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect)
{
	if ((Effect.Get() != nullptr) && (!AppliedEffects.Contains(Effect)))
	{
		FGlobalAppliedEffectList& Entry = AppliedEffects.Add(Effect);
		for (ULyraAbilitySystemComponent* ASC : RegisteredASCs)
		{
			Entry.AddToASC(Effect, ASC);
		}
	}
}

void ULyraGlobalAbilitySystem::RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability)
{
	if ((Ability.Get() != nullptr) && AppliedAbilities.Contains(Ability))
	{
		FGlobalAppliedAbilityList& Entry = AppliedAbilities[Ability];
		Entry.RemoveFromAll();
		AppliedAbilities.Remove(Ability);
	}
}

void ULyraGlobalAbilitySystem::RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect)
{
	if ((Effect.Get() != nullptr) && AppliedEffects.Contains(Effect))
	{
		FGlobalAppliedEffectList& Entry = AppliedEffects[Effect];
		Entry.RemoveFromAll();
		AppliedEffects.Remove(Effect);
	}
}

void ULyraGlobalAbilitySystem::ApplyDynamicTagToAll(FGameplayTag Tag)
{
	if (Tag.IsValid() && AppliedTags.Contains(Tag) == false)
	{
		AppliedTags.Add(Tag);
		for (ULyraAbilitySystemComponent* ASC : RegisteredASCs)
		{
			ASC->AddDynamicTagGameplayEffect(Tag);
		}
	}
}

void ULyraGlobalAbilitySystem::RemoveDynamicTagFromAll(FGameplayTag Tag)
{
	if (Tag.IsValid() && AppliedTags.Contains(Tag))
	{
		for (ULyraAbilitySystemComponent* ASC : RegisteredASCs)
		{
			ASC->RemoveDynamicTagGameplayEffect(Tag);
		}
		AppliedTags.Remove(Tag);
	}
}

void ULyraGlobalAbilitySystem::RegisterASC(ULyraAbilitySystemComponent* ASC)
{
	check(ASC);

	for (auto& Entry : AppliedAbilities)
	{
		Entry.Value.AddToASC(Entry.Key, ASC);
	}
	for (auto& Entry : AppliedEffects)
	{
		Entry.Value.AddToASC(Entry.Key, ASC);
	}
	for (const FGameplayTag& Tag : AppliedTags)
	{
		ASC->AddDynamicTagGameplayEffect(Tag);
	}

	RegisteredASCs.AddUnique(ASC);
}

void ULyraGlobalAbilitySystem::UnregisterASC(ULyraAbilitySystemComponent* ASC)
{
	check(ASC);
	for (auto& Entry : AppliedAbilities)
	{
		Entry.Value.RemoveFromASC(ASC);
	}
	for (auto& Entry : AppliedEffects)
	{
		Entry.Value.RemoveFromASC(ASC);
	}
	for (const FGameplayTag& Tag : AppliedTags)
	{
		ASC->RemoveDynamicTagGameplayEffect(Tag);
	}

	RegisteredASCs.Remove(ASC);
}
