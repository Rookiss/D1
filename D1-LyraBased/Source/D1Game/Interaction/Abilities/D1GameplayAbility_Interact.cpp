#include "D1GameplayAbility_Interact.h"

#include "AbilitySystemComponent.h"
#include "D1GameplayTags.h"
#include "AbilitySystem/Abilities/Tasks/D1AbilityTask_WaitInputStart.h"
#include "Character/LyraCharacter.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Interaction/D1InteractionInfo.h"
#include "Physics/LyraCollisionChannels.h"
#include "Tasks/D1AbilityTask_GrantNearbyInteraction.h"
#include "Tasks/D1AbilityTask_WaitForInteractableTraceHit.h"
#include "UI/IndicatorSystem/LyraIndicatorManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Interact)

UD1GameplayAbility_Interact::UD1GameplayAbility_Interact(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = ELyraAbilityActivationPolicy::OnSpawn;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UD1GameplayAbility_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FD1InteractionQuery InteractionQuery;
	InteractionQuery.RequestingAvatar = GetAvatarActorFromActorInfo();
	InteractionQuery.RequestingController = GetControllerFromActorInfo();
	
	if (UD1AbilityTask_WaitForInteractableTraceHit* TraceHitTask = UD1AbilityTask_WaitForInteractableTraceHit::WaitForInteractableTraceHit(this, InteractionQuery, D1_TraceChannel_Interaction, MakeTargetLocationInfoFromOwnerActor(), InteractionTraceRange, InteractionTraceRate, bShowTraceDebug))
	{
		TraceHitTask->InteractableChanged.AddDynamic(this, &ThisClass::UpdateInteractions);
		TraceHitTask->ReadyForActivation();
	}

	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	if (AbilitySystem && AbilitySystem->GetOwnerRole() == ROLE_Authority)
	{
		UD1AbilityTask_GrantNearbyInteraction* GrantAbilityTask = UD1AbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractables(this, InteractionScanRange, InteractionScanRate);
		GrantAbilityTask->ReadyForActivation();
	}
	
	WaitInputStart();
}

void UD1GameplayAbility_Interact::UpdateInteractions(const TArray<FD1InteractionInfo>& InteractionInfos)
{
	FD1InteractionMessage Message;
	Message.Instigator = GetAvatarActorFromActorInfo();
	Message.bShouldRefresh = true;
	Message.bSwitchActive = (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(D1GameplayTags::Status_Interact) == false);
	Message.InteractionInfo = InteractionInfos.Num() > 0 ? InteractionInfos[0] : FD1InteractionInfo();

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetAvatarActorFromActorInfo());
	MessageSystem.BroadcastMessage(D1GameplayTags::Message_Interaction_Notice, Message);

	CurrentInteractionInfos = InteractionInfos;
}

void UD1GameplayAbility_Interact::TriggerInteraction()
{
	if (CurrentInteractionInfos.Num() == 0)
		return;
	
	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetAvatarActorFromActorInfo());
	if (LyraCharacter && LyraCharacter->GetMovementComponent()->IsFalling())
		return;
	
	if (GetAbilitySystemComponentFromActorInfo())
	{
		const FD1InteractionInfo& InteractionInfo = CurrentInteractionInfos[0];

		AActor* Instigator = GetAvatarActorFromActorInfo();
		AActor* InteractableActor = nullptr;

		if (UObject* Object = InteractionInfo.Interactable.GetObject())
		{
			if (AActor* Actor = Cast<AActor>(Object))
			{
				InteractableActor = Actor;
			}
			else if (UActorComponent* ActorComponent = Cast<UActorComponent>(Object))
			{
				InteractableActor = ActorComponent->GetOwner();
			}
		}
		
		FGameplayEventData Payload;
		Payload.EventTag = D1GameplayTags::Ability_Interact_Active;
		Payload.Instigator = Instigator;
		Payload.Target = InteractableActor;
		
		SendGameplayEvent(D1GameplayTags::Ability_Interact_Active, Payload);
	}
}

void UD1GameplayAbility_Interact::WaitInputStart()
{
	if (UD1AbilityTask_WaitInputStart* InputStartTask = UD1AbilityTask_WaitInputStart::WaitInputStart(this))
	{
		InputStartTask->OnStart.AddDynamic(this, &ThisClass::OnInputStart);
		InputStartTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Interact::OnInputStart()
{
	TriggerInteraction();
	WaitInputStart();
}
