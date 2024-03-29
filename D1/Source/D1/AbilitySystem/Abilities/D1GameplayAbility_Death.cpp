#include "D1GameplayAbility_Death.h"

#include "D1GameplayTags.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Character/D1Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Death)

UD1GameplayAbility_Death::UD1GameplayAbility_Death(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityTags.AddTag(D1GameplayTags::Ability_Death);
	
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UD1GameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UD1AbilitySystemComponent* D1ASC = CastChecked<UD1AbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
	D1ASC->CancelAllAbilities();
	
	SetCanBeCanceled(false);
	StartDeath();
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UD1GameplayAbility_Death::StartDeath()
{
	if (AD1Character* Character = Cast<AD1Character>(GetAvatarActorFromActorInfo()))
	{
		Character->StartDeath();
	}
}

void UD1GameplayAbility_Death::FinishDeath()
{
	if (AD1Character* Character = Cast<AD1Character>(GetAvatarActorFromActorInfo()))
	{
		Character->FinishDeath();
	}
}
