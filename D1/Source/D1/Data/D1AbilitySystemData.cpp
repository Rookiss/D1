#include "D1AbilitySystemData.h"

#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/D1GameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AbilitySystemData)

void FD1AbilitySystemData_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FD1AbilitySystemData_GrantedHandles::AddEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		EffectHandles.Add(Handle);
	}
}

void FD1AbilitySystemData_GrantedHandles::TakeFromAbilitySystem(UD1AbilitySystemComponent* ASC)
{
	check(ASC);

	if (ASC->IsOwnerActorAuthoritative() == false)
		return;

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			ASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : EffectHandles)
	{
		if (Handle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	AbilitySpecHandles.Reset();
	EffectHandles.Reset();
}

UD1AbilitySystemData::UD1AbilitySystemData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1AbilitySystemData::GiveToAbilitySystem(UD1AbilitySystemComponent* ASC, FD1AbilitySystemData_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(ASC);

	if (ASC->IsOwnerActorAuthoritative() == false)
		return;

	for (int32 AbilityIndex = 0; AbilityIndex < Abilities.Num(); AbilityIndex++)
	{
		const FD1AbilitySystemData_Ability& AbilityToGrant = Abilities[AbilityIndex];
		if (IsValid(AbilityToGrant.Ability) == false)
		{
			UE_LOG(LogAbilitySystemComponent, Error, TEXT("GrantedAbilities[%d] on ability system data [%s] is not vaild."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		UD1GameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UD1GameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	for (int32 EffectIndex = 0; EffectIndex < Effects.Num(); EffectIndex++)
	{
		const FD1AbilitySystemData_Effect& EffectToGrant = Effects[EffectIndex];
		if (IsValid(EffectToGrant.Effect) == false)
		{
			UE_LOG(LogAbilitySystemComponent, Error, TEXT("GrantedEffect[%d] on ability system data [%s] is not vaild."), EffectIndex, *GetNameSafe(this));
			continue;
		}

		const UGameplayEffect* Effect = EffectToGrant.Effect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle EffectHandle = ASC->ApplyGameplayEffectToSelf(Effect, EffectToGrant.EffectLevel, ASC->MakeEffectContext());
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddEffectHandle(EffectHandle);
		}
	}
}
