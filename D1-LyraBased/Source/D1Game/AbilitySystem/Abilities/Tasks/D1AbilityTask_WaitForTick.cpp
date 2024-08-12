#include "D1AbilityTask_WaitForTick.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AbilityTask_WaitForTick)

UD1AbilityTask_WaitForTick::UD1AbilityTask_WaitForTick(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
}

UD1AbilityTask_WaitForTick* UD1AbilityTask_WaitForTick::WaitForTick(UGameplayAbility* OwningAbility)
{
	UD1AbilityTask_WaitForTick* Task = NewAbilityTask<UD1AbilityTask_WaitForTick>(OwningAbility);
	return Task;
}

void UD1AbilityTask_WaitForTick::Activate()
{
	Super::Activate();
}

void UD1AbilityTask_WaitForTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnTick.Broadcast(DeltaTime);
	}
}
