#include "D1Ability_Jump.h"

#include "Character/D1Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1Ability_Jump)

UD1Ability_Jump::UD1Ability_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UD1Ability_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (ActorInfo == nullptr || ActorInfo->AvatarActor.IsValid() == false)
		return false;

	const AD1Character* D1Character = Cast<AD1Character>(ActorInfo->AvatarActor.Get());
	if (D1Character == nullptr || D1Character->CanJump() == false)
		return false;

	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) == false)
		return false;

	return true;
}

void UD1Ability_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	StartJump();
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UD1Ability_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	StopJump();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1Ability_Jump::StartJump()
{
	if (AD1Character* D1Character = GetD1Character())
	{
		if (D1Character->IsLocallyControlled())
		{
			D1Character->UnCrouch();
			D1Character->Jump();
		}
	}
}

void UD1Ability_Jump::StopJump()
{
	if (AD1Character* D1Character = GetD1Character())
	{
		if (D1Character->IsLocallyControlled())
		{
			D1Character->StopJumping();
		}
	}
}
