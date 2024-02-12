#include "D1GameplayAbility_Interact.h"

#include "AbilitySystemComponent.h"
#include "D1GameplayTags.h"
#include "Interaction/D1Interactable.h"
#include "Player/D1PlayerController.h"
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

	GetWorld()->GetTimerManager().SetTimer(UpdateWidgetTimerHandle, this, &ThisClass::UpdateWidget, 0.1f, true);
}

void UD1GameplayAbility_Interact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	GetWorld()->GetTimerManager().ClearTimer(UpdateWidgetTimerHandle);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_Interact::UpdateLineTracedInfo(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	CurrentTargetDataHandle = TargetDataHandle;
}

void UD1GameplayAbility_Interact::UpdateWidget()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC && ASC->HasMatchingGameplayTag(D1GameplayTags::Ability_Interaction_Active))
		return;
		
	if (CurrentTargetDataHandle.Num() > 0)
	{
		if (ID1Interactable* TargetInteractable = Cast<ID1Interactable>(CurrentTargetDataHandle.Get(0)->GetHitResult()->GetActor()))
		{
			ShowInteractionPressWidget(TargetInteractable->GetInteractionInfo());
			return;
		}
	}
		
	HideInteractionWidget();
}

void UD1GameplayAbility_Interact::HandleInputPress()
{
	if (CurrentTargetDataHandle.Num() == 0 || CurrentTargetDataHandle.Get(0)->GetHitResult()->GetActor() == nullptr)
		return;
	
	FGameplayEventData Payload;
	Payload.Instigator = GetAvatarActorFromActorInfo();
	Payload.TargetData = CurrentTargetDataHandle;
	
	SendGameplayEvent(D1GameplayTags::Ability_Interaction_Active, Payload);
}

void UD1GameplayAbility_Interact::ShowInteractionPressWidget(const FD1InteractionInfo& InteractionInfo)
{
	if (AD1PlayerController* PC = GetPlayerController())
	{
		if (AD1HUD* HUD = Cast<AD1HUD>(PC->GetHUD()))
		{
			HUD->ShowInteractionPressWidget(InteractionInfo.InteractionTitle, InteractionInfo.InteractionContent);
		}
	}
}

void UD1GameplayAbility_Interact::HideInteractionWidget()
{
	if (AD1PlayerController* PC = GetPlayerController())
	{
		if (AD1HUD* HUD = Cast<AD1HUD>(PC->GetHUD()))
		{
			HUD->HideInteractionWidget();
		}
	}
}
