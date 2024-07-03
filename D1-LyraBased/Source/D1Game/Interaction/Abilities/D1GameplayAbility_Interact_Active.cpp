#include "D1GameplayAbility_Interact_Active.h"

#include "AbilitySystemComponent.h"
#include "D1GameplayAbility_Interact.h"
#include "D1GameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Interaction/D1Interactable.h"
#include "Interaction/D1InteractionQuery.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Interact_Active)

UD1GameplayAbility_Interact_Active::UD1GameplayAbility_Interact_Active(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = ELyraAbilityActivationPolicy::Manual;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;
	bServerRespectsRemoteAbilityCancellation = false;
}

void UD1GameplayAbility_Interact_Active::Initialize(AActor* TargetActor)
{
	TScriptInterface<ID1Interactable> TargetInteractable(TargetActor);
	if (TargetInteractable)
	{
		FD1InteractionQuery InteractionQuery;
		InteractionQuery.RequestingAvatar = GetAvatarActorFromActorInfo();
		InteractionQuery.RequestingController = GetControllerFromActorInfo();

		Interactable = TargetInteractable;
		InteractableActor = TargetActor;

		TArray<FD1InteractionInfo> InteractionInfos;
		FD1InteractionInfoBuilder InteractionInfoBuilder(Interactable, InteractionInfos);
		Interactable->GatherPostInteractionInfos(InteractionQuery, InteractionInfoBuilder);
		InteractionInfo = InteractionInfos[0];
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UD1GameplayAbility_Interact_Active::TriggerInteraction()
{
	FGameplayEventData Payload;
	Payload.EventTag = D1GameplayTags::Ability_Interact;
	Payload.Instigator = GetAvatarActorFromActorInfo();
	Payload.Target = InteractableActor;

	Interactable->CustomizeInteractionEventData(D1GameplayTags::Ability_Interact, Payload);

	AActor* TargetActor = const_cast<AActor*>(ToRawPtr(Payload.Target));

	if (UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo())
	{
		if (FGameplayAbilitySpec* InteractionAbilitySpec = AbilitySystem->FindAbilitySpecFromClass(InteractionInfo.InteractionAbilityToGrant))
		{
			FGameplayAbilityActorInfo ActorInfo;
			ActorInfo.InitFromActor(InteractableActor, TargetActor, AbilitySystem);
		
			AbilitySystem->TriggerAbilityFromGameplayEvent(
				InteractionAbilitySpec->Handle,
				&ActorInfo,
				D1GameplayTags::Ability_Interact,
				&Payload,
				*AbilitySystem
			);
		}
	}
}

void UD1GameplayAbility_Interact_Active::RefreshUI(bool bShouldRefresh, bool bShouldActive)
{
	FD1InteractionMessage Message;
	Message.Instigator = GetAvatarActorFromActorInfo();
	Message.bShouldRefresh = bShouldRefresh;
	Message.bShouldActive = bShouldActive;
	Message.InteractionInfo = InteractionInfo;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetAvatarActorFromActorInfo());
	MessageSystem.BroadcastMessage(D1GameplayTags::Message_Interaction_Progress, Message);

	if (bShouldRefresh == false)
	{
		Message.bShouldRefresh = false;
		Message.bShouldActive = true;
		MessageSystem.BroadcastMessage(D1GameplayTags::Message_Interaction_Notice, Message);
	}
}
