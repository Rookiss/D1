#include "D1ActivateAbilitiesGameplayEffectComponent.h"

#include "AbilitySystemComponent.h"
#include "D1LogChannels.h"
#include "GameplayEffect.h"
#include "GameplayEffectComponents/AbilitiesGameplayEffectComponent.h"
#include "Misc/DataValidation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ActivateAbilitiesGameplayEffectComponent)

#define LOCTEXT_NAMESPACE "ActivateAbilitiesGameplayEffectComponent"

UD1ActivateAbilitiesGameplayEffectComponent::UD1ActivateAbilitiesGameplayEffectComponent()
{
#if WITH_EDITORONLY_DATA
	EditorFriendlyName = TEXT("Activate Gameplay Abilities by Event");
#endif
}

bool UD1ActivateAbilitiesGameplayEffectComponent::OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer, FActiveGameplayEffect& ActiveGE) const
{
	if (ActiveGEContainer.IsNetAuthority())
	{
		ActiveGE.EventSet.OnEffectRemoved.AddUObject(this, &ThisClass::OnActiveGameplayEffectRemoved, &ActiveGEContainer);
	}

	return true;
}

void UD1ActivateAbilitiesGameplayEffectComponent::OnActiveGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo, FActiveGameplayEffectsContainer* ActiveGEContainer) const
{
	const FActiveGameplayEffect* ActiveGE = RemovalInfo.ActiveEffect;
	if (ensure(ActiveGE) == false)
		return;

	UAbilitySystemComponent* ASC = ActiveGEContainer->Owner;
	if (ensure(ASC) == false)
		return;

	for (FGameplayAbilityActivateConfig ActivateAbilityConfig : ActivateAbilityConfigs)
	{
		if (ActivateAbilityConfig.CancelPolicy != EGameplayEffectActivatedAbilityCancelPolicy::CancelOnEffectEnd)
			continue;

		if (const FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromClass(ActivateAbilityConfig.AbilityClass))
		{
			ASC->CancelAbilityHandle(AbilitySpec->Handle);
		}
	}
}

void UD1ActivateAbilitiesGameplayEffectComponent::OnGameplayEffectApplied(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const
{
	UAbilitySystemComponent* ASC = ActiveGEContainer.Owner;
	if (ensure(ASC) == false)
		return;
	
	for (const FGameplayAbilityActivateConfig& ActivateAbilityConfig : ActivateAbilityConfigs)
	{
		FGameplayEventData Payload;
		Payload.Instigator = GESpec.GetEffectContext().GetInstigator();
		Payload.ContextHandle = GESpec.GetEffectContext();
		
		ASC->HandleGameplayEvent(ActivateAbilityConfig.AbilityEventTag, &Payload);
	}
}

#if WITH_EDITOR
EDataValidationResult UD1ActivateAbilitiesGameplayEffectComponent::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	for (const FGameplayAbilityActivateConfig& ActivateAbilityConfig : ActivateAbilityConfigs)
	{
		if (ActivateAbilityConfig.AbilityClass == nullptr)
		{
			Context.AddError(FText::FormatOrdered(LOCTEXT("InvalidAbilityClass", "AbilityClass is Invalid: {0}."), FText::FromString(GetClass()->GetName())));
			Result = EDataValidationResult::Invalid;
		}
		else
		{
			if (const UGameplayAbility* AbilityCDO = ActivateAbilityConfig.AbilityClass.GetDefaultObject())
			{
				if (AbilityCDO->GetNetExecutionPolicy() < EGameplayAbilityNetExecutionPolicy::Type::ServerInitiated)
				{
					Context.AddError(FText::FormatOrdered(LOCTEXT("InvalidNetExecutionPolicy", "NetExecutionPolicy must be ServerInitiated or ServerOnly: {0}."), FText::FromString(GetClass()->GetName())));
					Result = EDataValidationResult::Invalid;
				}
			}
		}
		
		if (ActivateAbilityConfig.AbilityEventTag.IsValid() == false)
		{
			Context.AddError(FText::FormatOrdered(LOCTEXT("InvalidAbilityEventTag", "AbilityEventTag is Invalid: {0}."), FText::FromString(GetClass()->GetName())));
			Result = EDataValidationResult::Invalid;
		}

		if (GetOwner()->DurationPolicy == EGameplayEffectDurationType::Instant)
		{
			if (ActivateAbilityConfig.CancelPolicy == EGameplayEffectActivatedAbilityCancelPolicy::CancelOnEffectEnd)
			{
				Context.AddError(FText::FormatOrdered(LOCTEXT("InstantDoesNotWorkWithCancelOnEffectEnd", "CancelOnEffectEnd does not work with Instant Effects: {0}."), FText::FromString(GetClass()->GetName())));
				Result = EDataValidationResult::Invalid;
			}
		}
	}
	
	for (int32 Index = 0; Index < ActivateAbilityConfigs.Num(); Index++)
	{
		const FGameplayTag& AbilityEventTag = ActivateAbilityConfigs[Index].AbilityEventTag;
		for (int CheckIndex = Index + 1; CheckIndex < ActivateAbilityConfigs.Num(); CheckIndex++)
		{
			if (AbilityEventTag == ActivateAbilityConfigs[CheckIndex].AbilityEventTag)
			{
				Context.AddError(FText::FormatOrdered(LOCTEXT("ActivateAbilitiesMustBeUnique", "Multiple ActivateAbility of the same tag cannot be activated by {0}."), FText::FromString(GetClass()->GetName())));
				Result = EDataValidationResult::Invalid;
			}
		}
	}

	return Result;
}
#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
