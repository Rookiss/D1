#include "D1AbilityTask_WaitForTagAddedOrRemoved.h"

#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AbilityTask_WaitForTagAddedOrRemoved)

UD1AbilityTask_WaitForTagAddedOrRemoved::UD1AbilityTask_WaitForTagAddedOrRemoved(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

UD1AbilityTask_WaitForTagAddedOrRemoved* UD1AbilityTask_WaitForTagAddedOrRemoved::WaitForTagAddedOrRemoved(UGameplayAbility* OwningAbility, FGameplayTagContainer InListenTags)
{
	UD1AbilityTask_WaitForTagAddedOrRemoved* Task = NewAbilityTask<UD1AbilityTask_WaitForTagAddedOrRemoved>(OwningAbility);
	Task->ListenTags = InListenTags;
	return Task;
}

void UD1AbilityTask_WaitForTagAddedOrRemoved::Activate()
{
	Super::Activate();

	for (const FGameplayTag& Tag : ListenTags)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnTagChanged);
	}
}

void UD1AbilityTask_WaitForTagAddedOrRemoved::OnDestroy(bool bInOwnerFinished)
{
	for (const FGameplayTag& Tag : ListenTags)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

void UD1AbilityTask_WaitForTagAddedOrRemoved::OnTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	(NewCount > 0) ? OnTagAdded.Broadcast(Tag) : OnTagRemoved.Broadcast(Tag);
}
