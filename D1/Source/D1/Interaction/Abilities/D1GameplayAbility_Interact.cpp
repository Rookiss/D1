#include "D1GameplayAbility_Interact.h"

#include "AbilitySystemComponent.h"
#include "D1GameplayTags.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "Character/D1Character.h"
#include "Interaction/D1Interactable.h"
#include "Interaction/Tasks/D1AbilityTask_GrantNearbyInteractionAbilities.h"
#include "Player/D1PlayerController.h"
#include "UI/D1HUD.h"

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

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC && ASC->GetOwnerRole() == ROLE_Authority)
	{
		UD1AbilityTask_GrantNearbyInteractionAbilities* Task = UD1AbilityTask_GrantNearbyInteractionAbilities::GrantNearbyInteractionAbilities(this, InteractionScanRange, InteractionScanRate);
		Task->ReadyForActivation();
	}
}

void UD1GameplayAbility_Interact::HandleInteractionInfo(const FD1InteractionInfo& NewInteractionInfo)
{
	if (TraceHitInfo != NewInteractionInfo)
	{
		TraceHitInfo = NewInteractionInfo;
		
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		UD1AbilitySystemComponent* ASC = CastChecked<UD1AbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
		
		if (TimerManager.TimerExists(HoldTimerHandle))
		{
			ASC->RemoveDynamicTagToSelf(D1GameplayTags::ASC_InputBlocked);
			TimerManager.ClearTimer(HoldTimerHandle);
		}

		if (GetAvatarActorFromActorInfo()->HasAuthority() == false && TraceHitInfo.Interactable.IsValid())
		{
			ShowInteractionPress(TraceHitInfo.InteractionTitle, TraceHitInfo.InteractionContent);
		}
		else
		{
			HideInteractionWidget();
		}
	}
}

void UD1GameplayAbility_Interact::OnInteractionPressDetected()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	UD1AbilitySystemComponent* ASC = CastChecked<UD1AbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	
	if (TimerManager.TimerExists(HoldTimerHandle))
	{
		ASC->RemoveDynamicTagToSelf(D1GameplayTags::ASC_InputBlocked);
		ShowInteractionPress(TraceHitInfo.InteractionTitle, TraceHitInfo.InteractionContent);
		TimerManager.ClearTimer(HoldTimerHandle);
		return;
	}

	if (TraceHitInfo.Interactable.IsValid())
	{
		ASC->AddDynamicTagToSelf(D1GameplayTags::ASC_InputBlocked);
		ShowInteractionProgress(TraceHitInfo.HoldTime);
		TimerManager.SetTimer(HoldTimerHandle, this, &ThisClass::TriggerInteraction, TraceHitInfo.HoldTime, false);
	}
}

void UD1GameplayAbility_Interact::TriggerInteraction()
{
	if (UD1AbilitySystemComponent* ASC = Cast<UD1AbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
	{
		ASC->RemoveDynamicTagToSelf(D1GameplayTags::ASC_InputBlocked);
		ShowInteractionPress(TraceHitInfo.InteractionTitle, TraceHitInfo.InteractionContent);
		
		AActor* Instigator = GetAvatarActorFromActorInfo();
		AActor* InteractableActor = Cast<AActor>(TraceHitInfo.Interactable.GetObject());
		
		FGameplayEventData Payload;
		Payload.EventTag = D1GameplayTags::Ability_Interaction;
		Payload.Instigator = Instigator;
		Payload.Target = InteractableActor;

		ASC->TriggerAbilityFromGameplayEvent(TraceHitInfo.InteractionAbilityHandle, nullptr, D1GameplayTags::Ability_Interaction, &Payload, *ASC);
	}
}

void UD1GameplayAbility_Interact::ShowInteractionPress(const FText& InteractionTitle, const FText& InteractionContent)
{
	if (AD1PlayerController* PC = GetPlayerController())
	{
		if (AD1HUD* HUD = Cast<AD1HUD>(PC->GetHUD()))
		{
			HUD->ShowInteractionPress(InteractionTitle, InteractionContent);
		}
	}
}

void UD1GameplayAbility_Interact::ShowInteractionProgress(float HoldTime)
{
	if (AD1PlayerController* PC = GetPlayerController())
	{
		if (AD1HUD* HUD = Cast<AD1HUD>(PC->GetHUD()))
		{
			HUD->ShowInteractionProgress(HoldTime);
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
