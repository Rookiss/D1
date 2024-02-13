#include "D1GameplayAbility_Interact_Active.h"

#include "AbilitySystemComponent.h"
#include "Interaction/D1Interactable.h"
#include "UI/D1HUD.h"
#include "UI/Interaction/D1InteractionWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Interact_Active)

UD1GameplayAbility_Interact_Active::UD1GameplayAbility_Interact_Active(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = ED1AbilityActivationPolicy::Manual;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UD1GameplayAbility_Interact_Active::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	HideInteractionDurationWidget();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_Interact_Active::Initialize(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	AActor* TargetActor = TargetDataHandle.Get(0)->GetHitResult()->GetActor();
	
	if (ID1Interactable* Interactable = Cast<ID1Interactable>(TargetActor))
	{
		CurrentTargetDataHandle = TargetDataHandle;
		CachedTargetActor = TargetActor;
		CachedHoldTime = Interactable->GetInteractionInfo().HoldTime;
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UD1GameplayAbility_Interact_Active::Interact()
{
	ID1Interactable::Execute_Interact(CachedTargetActor.Get(), GetAvatarActorFromActorInfo());
}

void UD1GameplayAbility_Interact_Active::ShowInteractionDurationWidget()
{
	if (AD1HUD* HUD = GetHUD())
	{
		if (UD1InteractionWidget* InteractionWidget = HUD->GetInteractionWidget())
		{
			InteractionWidget->ShowInteractionDurationWidget(CachedHoldTime);
		}
	}
}

void UD1GameplayAbility_Interact_Active::HideInteractionDurationWidget()
{
	if (AD1HUD* HUD = GetHUD())
	{
		if (UD1InteractionWidget* InteractionWidget = HUD->GetInteractionWidget())
		{
			InteractionWidget->HideInteractionWidget();
		}
	}
}
