#include "D1GameplayAbility_Interact.h"

#include "AbilitySystemComponent.h"
#include "D1GameplayTags.h"
#include "Interaction/D1InteractionInfo.h"
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

void UD1GameplayAbility_Interact::RefreshInteractionWidget(const FD1InteractionInfo& InteractionInfo)
{
	if (AD1PlayerController* PC = GetPlayerController())
	{
		if (AD1HUD* HUD = Cast<AD1HUD>(PC->GetHUD()))
		{
			if (InteractionInfo.Interactable)
			{
				HUD->ShowInteractionWidget(InteractionInfo.InteractionText);
			}
			else
			{
				HUD->HideInteractionWidget();
			}
		}
	}

	LatestInfo = InteractionInfo;
}

void UD1GameplayAbility_Interact::TriggerInteraction()
{
	if (LatestInfo.Interactable == nullptr)
		return;
	
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		AActor* Instigator = GetAvatarActorFromActorInfo();
		AActor* InteractableActor = Cast<AActor>(LatestInfo.Interactable.GetObject());
		
		FGameplayEventData Payload;
		Payload.EventTag = D1GameplayTags::Ability_Interaction_Activate;
		Payload.Instigator = Instigator;
		Payload.Target = InteractableActor;
	
		FGameplayAbilityActorInfo ActorInfo;
		ActorInfo.InitFromActor(InteractableActor, InteractableActor, ASC);

		ASC->TriggerAbilityFromGameplayEvent(
			LatestInfo.InteractionAbilityHandle,
			&ActorInfo,
			D1GameplayTags::Ability_Interaction_Activate,
			&Payload,
			*ASC
		);
	}
}
