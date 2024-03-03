#include "D1GameplayAbility_ComboAttack.h"

#include "D1GameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_ComboAttack)

UD1GameplayAbility_ComboAttack::UD1GameplayAbility_ComboAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_ComboAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	const FGameplayAbilitySpec* AbilitySpec = GetCurrentAbilitySpec();
	if (AbilitySpec->DynamicAbilityTags.HasTagExact(D1GameplayTags::Input_Action_Attack_MainHand))
	{
		
	}
	else if (AbilitySpec->DynamicAbilityTags.HasTagExact(D1GameplayTags::Input_Action_Attack_OffHand))
	{
		
	}
}
