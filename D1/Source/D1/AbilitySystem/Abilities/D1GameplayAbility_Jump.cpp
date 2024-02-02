#include "D1GameplayAbility_Jump.h"

#include "D1GameplayTags.h"
#include "Character/D1Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Jump)

UD1GameplayAbility_Jump::UD1GameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityTags.AddTag(D1GameplayTags::Ability_Jump);
	
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UD1GameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (ActorInfo == nullptr || ActorInfo->AvatarActor.IsValid() == false)
		return false;

	const AD1Character* Character = Cast<AD1Character>(ActorInfo->AvatarActor.Get());
	if (Character == nullptr || Character->CanJump() == false)
		return false;

	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) == false)
		return false;

	return true;
}

void UD1GameplayAbility_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	StartJump();
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UD1GameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	StopJump();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_Jump::StartJump()
{
	if (AD1Character* Character = GetCharacter())
	{
		if (Character->IsLocallyControlled())
		{
			Character->Jump();
		}
	}
}

void UD1GameplayAbility_Jump::StopJump()
{
	if (AD1Character* Character = GetCharacter())
	{
		if (Character->IsLocallyControlled())
		{
			Character->StopJumping();
		}
	}
}
