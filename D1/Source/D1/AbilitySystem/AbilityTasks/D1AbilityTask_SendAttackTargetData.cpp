#include "D1AbilityTask_SendAttackTargetData.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/D1GameplayAbility_Weapon.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AbilityTask_SendAttackTargetData)

UD1AbilityTask_SendAttackTargetData* UD1AbilityTask_SendAttackTargetData::SendAttackTargetData(UGameplayAbility* OwningAbility)
{
	UD1AbilityTask_SendAttackTargetData* NewTask = NewAbilityTask<UD1AbilityTask_SendAttackTargetData>(OwningAbility);
	return NewTask;
}

void UD1AbilityTask_SendAttackTargetData::Activate()
{
	Super::Activate();

	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		SendTargetDataToServer();
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &ThisClass::OnTargetDataSet);

		const bool bDelegateCalled = AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if (bDelegateCalled == false)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UD1AbilityTask_SendAttackTargetData::SendTargetDataToServer()
{
	FScopedPredictionWindow PredictionWindow(AbilitySystemComponent.Get());
	
	FGameplayAbilityTargetDataHandle TargetDataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit();
	
	// Get TargetData From Ability
	TargetData->HitResult;

	AbilitySystemComponent->ServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(), TargetDataHandle, FGameplayTag(), AbilitySystemComponent->ScopedPredictionKey);
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		TargetDataSet.Broadcast(TargetDataHandle);
	}
}

void UD1AbilityTask_SendAttackTargetData::OnTargetDataSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		TargetDataSet.Broadcast(TargetDataHandle);
	}
}
