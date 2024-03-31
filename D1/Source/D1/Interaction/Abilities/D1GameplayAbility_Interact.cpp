#include "D1GameplayAbility_Interact.h"

#include "AbilitySystemComponent.h"
#include "D1GameplayAbility_Interact_Active.h"
#include "D1GameplayTags.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Character/D1Character.h"
#include "Interaction/D1Interactable.h"
#include "UI/D1HUD.h"
#include "UI/Interaction/D1InteractionWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Interact)

UD1GameplayAbility_Interact::UD1GameplayAbility_Interact(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = ED1AbilityActivationPolicy::OnGiveOrSpawn;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UD1GameplayAbility_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthority(&ActivationInfo) == false)
	{
		GetWorld()->GetTimerManager().SetTimer(UpdateWidgetTimerHandle, this, &ThisClass::UpdateWidget, 0.05f, true);
	}
}

void UD1GameplayAbility_Interact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (HasAuthority(&ActivationInfo) == false)
	{
		GetWorld()->GetTimerManager().ClearTimer(UpdateWidgetTimerHandle);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_Interact::HandleInputPress()
{
	if (CurrentTargetDataHandle.Num() <= 0)
		return;
	
	AActor* TargetActor = CurrentTargetDataHandle.Get(0)->GetHitResult()->GetActor();
	if (TargetActor == nullptr)
		return;
	
	FGameplayEventData Payload;
	Payload.Instigator = GetAvatarActorFromActorInfo();
	Payload.TargetData = CurrentTargetDataHandle;
	
	SendGameplayEvent(D1GameplayTags::Event_Interact, Payload);
}

void UD1GameplayAbility_Interact::UpdateWidget()
{
	if (FGameplayAbilitySpec* AbilitySpec = GetAbilitySystemComponent()->FindAbilitySpecFromClass(InteractActiveAbilityClass))
	{
		if (AbilitySpec->Ability->CanActivateAbility(AbilitySpec->Handle, GetCurrentActorInfo()) == false)
		{
			HideInteractionWidget();
			return;
		}
	}
	
	if (CurrentTargetDataHandle.Num() <= 0)
		return;

	AD1HUD* HUD = GetHUD();
	if (HUD == nullptr)
		return;

	UD1InteractionWidget* InteractionWidget = HUD->GetInteractionWidget();
	if (InteractionWidget && InteractionWidget->IsShowingDurationWidget() == false)
	{
		AActor* TargetActor = CurrentTargetDataHandle.Get(0)->GetHitResult()->GetActor();
		if (ID1Interactable* TargetInteractable = Cast<ID1Interactable>(TargetActor))
		{
			ShowInteractionPressWidget(TargetInteractable->GetInteractionInfo());
		}
		else
		{
			HideInteractionWidget();
		}
	}
}

void UD1GameplayAbility_Interact::ShowInteractionPressWidget(const FD1InteractionInfo& InteractionInfo)
{
	if (AD1HUD* HUD = GetHUD())
	{
		if (UD1InteractionWidget* InteractionWidget = HUD->GetInteractionWidget())
		{
			InteractionWidget->ShowInteractionPressWidget(InteractionInfo.InteractionTitle, InteractionInfo.InteractionContent);
		}
	}
}

void UD1GameplayAbility_Interact::HideInteractionWidget()
{
	if (AD1HUD* HUD = GetHUD())
	{
		if (UD1InteractionWidget* InteractionWidget = HUD->GetInteractionWidget())
		{
			InteractionWidget->HideInteractionWidget();
		}
	}
}
