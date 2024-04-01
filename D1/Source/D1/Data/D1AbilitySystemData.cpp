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
		GrantedAbilitySpecHandles.Add(Handle);
	}
}

void FD1AbilitySystemData_GrantedHandles::AddEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GrantedEffectHandles.Add(Handle);
	}
}

void FD1AbilitySystemData_GrantedHandles::AddAttributeSet(UAttributeSet* AttributeSet)
{
	GrantedAttributeSets.Add(AttributeSet);
}

void FD1AbilitySystemData_GrantedHandles::TakeFromAbilitySystem(UD1AbilitySystemComponent* ASC)
{
	check(ASC);

	if (ASC->IsOwnerActorAuthoritative() == false)
		return;

	for (const FGameplayAbilitySpecHandle& Handle : GrantedAbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			ASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GrantedEffectHandles)
	{
		if (Handle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* AttributeSet : GrantedAttributeSets)
	{
		ASC->RemoveSpawnedAttribute(AttributeSet);
	}

	GrantedAbilitySpecHandles.Reset();
	GrantedEffectHandles.Reset();
	GrantedAttributeSets.Reset();
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

	// AttributeSet
	for (int32 AttributeSetIndex = 0; AttributeSetIndex < AttributeSetsToGrant.Num(); AttributeSetIndex++)
	{
		const FD1AbilitySystemData_AttributeSet& AttributeSetToGrant = AttributeSetsToGrant[AttributeSetIndex];

		if (IsValid(AttributeSetToGrant.AttributeSetClass) == false)
		{
			UE_LOG(LogAbilitySystemComponent, Error, TEXT("AttributeSetsToGrant[%d] ability system data [%s] is not valid"), AttributeSetIndex, *GetNameSafe(this));
			continue;
		}

		UAttributeSet* NewAttributeSet = NewObject<UAttributeSet>(ASC->GetOwner(), AttributeSetToGrant.AttributeSetClass);
		ASC->AddAttributeSetSubobject(NewAttributeSet);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewAttributeSet);
		}
	}

	// Effect
	for (int32 EffectIndex = 0; EffectIndex < EffectsToGrant.Num(); EffectIndex++)
	{
		const FD1AbilitySystemData_Effect& EffectToGrant = EffectsToGrant[EffectIndex];
		if (IsValid(EffectToGrant.EffectClass) == false)
		{
			UE_LOG(LogAbilitySystemComponent, Error, TEXT("EffectsToGrant[%d] on ability system data [%s] is not vaild."), EffectIndex, *GetNameSafe(this));
			continue;
		}

		const UGameplayEffect* Effect = EffectToGrant.EffectClass->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle EffectHandle = ASC->ApplyGameplayEffectToSelf(Effect, EffectToGrant.EffectLevel, ASC->MakeEffectContext());
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddEffectHandle(EffectHandle);
		}
	}
	
	// Ability
	for (int32 AbilityIndex = 0; AbilityIndex < AbilitiesToGrant.Num(); AbilityIndex++)
	{
		const FD1AbilitySystemData_Ability& AbilityToGrant = AbilitiesToGrant[AbilityIndex];
		if (IsValid(AbilityToGrant.AbilityClass) == false)
		{
			UE_LOG(LogAbilitySystemComponent, Error, TEXT("AbilitiesToGrant[%d] on ability system data [%s] is not vaild."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		UD1GameplayAbility* AbilityCDO = AbilityToGrant.AbilityClass->GetDefaultObject<UD1GameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
}

void UD1AbilitySystemData::GivePendingAbility(UD1AbilitySystemComponent* ASC, FGameplayAbilitySpecHandle* OutAbilitySpecHandle, int32 AbilityIndex, UObject* SourceObject) const
{
	if (AbilitiesToPending.IsValidIndex(AbilityIndex) == false)
		return;

	const FD1AbilitySystemData_Ability& AbilityToPending = AbilitiesToPending[AbilityIndex];
	if (IsValid(AbilityToPending.AbilityClass) == false)
	{
		UE_LOG(LogAbilitySystemComponent, Error, TEXT("AbilitiesToPending[%d] on ability system data [%s] is not vaild."), AbilityIndex, *GetNameSafe(this));
		return;
	}

	UD1GameplayAbility* AbilityCDO = AbilityToPending.AbilityClass->GetDefaultObject<UD1GameplayAbility>();

	FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToPending.AbilityLevel);
	AbilitySpec.SourceObject = SourceObject;
	AbilitySpec.DynamicAbilityTags.AddTag(AbilityToPending.InputTag);
	
	const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
	if (OutAbilitySpecHandle)
	{
		*OutAbilitySpecHandle = AbilitySpecHandle;
	}
}
