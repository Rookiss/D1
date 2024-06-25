#include "D1AbilityTask_WaitInputStart.h"

#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AbilityTask_WaitInputStart)

UD1AbilityTask_WaitInputStart::UD1AbilityTask_WaitInputStart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StartTime = 0.f;
}

UD1AbilityTask_WaitInputStart* UD1AbilityTask_WaitInputStart::WaitInputStart(UGameplayAbility* OwningAbility)
{
	UD1AbilityTask_WaitInputStart* Task = NewAbilityTask<UD1AbilityTask_WaitInputStart>(OwningAbility);
	return Task;
}

void UD1AbilityTask_WaitInputStart::Activate()
{
	StartTime = GetWorld()->GetTimeSeconds();

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC && Ability)
	{
		DelegateHandle = ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey()).AddUObject(this, &UD1AbilityTask_WaitInputStart::OnStartCallback);
		if (IsForRemoteClient())
		{
			if (ASC->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey()) == false)
			{
				SetWaitingOnRemotePlayerData();
			}
		}
	}
}

void UD1AbilityTask_WaitInputStart::OnStartCallback()
{
	float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime;

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (Ability == nullptr || ASC == nullptr)
		return;

	ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);

	FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());

	if (IsPredictingClient())
	{
		ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey(), ASC->ScopedPredictionKey);
	}
	else
	{
		ASC->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey());
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnStart.Broadcast(ElapsedTime);	
	}
	EndTask();
}
