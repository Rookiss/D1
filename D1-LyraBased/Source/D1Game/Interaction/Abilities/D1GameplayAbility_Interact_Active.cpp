#include "D1GameplayAbility_Interact_Active.h"

#include "AbilitySystemComponent.h"
#include "D1GameplayAbility_Interact.h"
#include "D1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Actors/D1EquipmentBase.h"
#include "Character/LyraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Interaction/D1Interactable.h"
#include "Interaction/D1WorldInteractable.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Tasks/D1AbilityTask_WaitForInvalidInteraction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Interact_Active)

UD1GameplayAbility_Interact_Active::UD1GameplayAbility_Interact_Active(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = ELyraAbilityActivationPolicy::Manual;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bServerRespectsRemoteAbilityCancellation = true;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;

	AbilityTags.AddTag(D1GameplayTags::Ability_Interact_Active);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_Interact);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = D1GameplayTags::Ability_Interact_Active;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UD1GameplayAbility_Interact_Active::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	if (InitializeAbility(const_cast<AActor*>(TriggerEventData->Target.Get())) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (AD1WorldInteractable* WorldInteractable = Cast<AD1WorldInteractable>(InteractableActor))
	{
		WorldInteractable->OnInteractActiveStarted(GetLyraCharacterFromActorInfo());
	}

	if (InteractionInfo.Duration <= 0.f)
	{
		TriggerInteraction();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	FlushPressedInput(MoveInputAction);
	
	if (ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo())
	{
		if (UCharacterMovementComponent* CharacterMovement = LyraCharacter->GetCharacterMovement())
		{
			CharacterMovement->StopMovementImmediately();
		}

		if (UD1EquipManagerComponent* EquipManager = LyraCharacter->GetComponentByClass<UD1EquipManagerComponent>())
		{
			EquipManager->ChangeShouldHiddenEquipments(true);
		}
	}

	FGameplayCueParameters Parameters;
	Parameters.Instigator = InteractableActor;
	K2_AddGameplayCueWithParams(InteractionInfo.ActiveLoopGameplayCueTag, Parameters, true);

	FD1InteractionMessage Message;
	Message.Instigator = GetAvatarActorFromActorInfo();
	Message.bShouldRefresh = true;
	Message.bSwitchActive = true;
	Message.InteractionInfo = InteractionInfo;
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_Interaction_Progress, Message);

	if (UAnimMontage* ActiveStartMontage = InteractionInfo.ActiveStartMontage)
	{
		if (UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("InteractMontage"), ActiveStartMontage, 1.f, NAME_None, true, 1.f, 0.f, false))
		{
			PlayMontageTask->ReadyForActivation();
		}
	}
	
	if (UD1AbilityTask_WaitForInvalidInteraction* InvalidInteractionTask = UD1AbilityTask_WaitForInvalidInteraction::WaitForInvalidInteraction(this, AcceptanceAngle, AcceptanceDistance))
	{
		InvalidInteractionTask->OnInvalidInteraction.AddDynamic(this, &ThisClass::OnInvalidInteraction);
		InvalidInteractionTask->ReadyForActivation();
	}

	if (UAbilityTask_WaitInputRelease* InputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false))
	{
		InputReleaseTask->OnRelease.AddDynamic(this, &ThisClass::OnInputReleased);
		InputReleaseTask->ReadyForActivation();
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::OnDurationEnded, InteractionInfo.Duration, false);
}

void UD1GameplayAbility_Interact_Active::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo())
	{
		if (bWasCancelled)
		{
			if (UD1EquipManagerComponent* EquipManager = LyraCharacter->GetComponentByClass<UD1EquipManagerComponent>())
			{
				EquipManager->ChangeShouldHiddenEquipments(false);

				if (EquipManager->GetCurrentEquipState() != EEquipState::Unarmed)
				{
					if (AD1EquipmentBase* EquippedActor = EquipManager->GetFirstEquippedActor())
					{
						if (UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("EquipMontage"), EquippedActor->GetEquipMontage(), 1.f, NAME_None, false, 1.f, 0.f, false))
						{
							PlayMontageTask->ReadyForActivation();
						}
					}
				}
			}
		}

		if (AD1WorldInteractable* WorldInteractable = Cast<AD1WorldInteractable>(InteractableActor))
		{
			WorldInteractable->OnInteractActiveEnded(LyraCharacter);
		}
	
		FD1InteractionMessage Message;
		Message.Instigator = LyraCharacter;
		Message.bShouldRefresh = false;
		Message.bSwitchActive = true;
			
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
		MessageSubsystem.BroadcastMessage(D1GameplayTags::Message_Interaction_Notice, Message);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_Interact_Active::OnInvalidInteraction()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UD1GameplayAbility_Interact_Active::OnInputReleased(float TimeHeld)
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UD1GameplayAbility_Interact_Active::OnDurationEnded()
{
	if (UAbilityTask_NetworkSyncPoint* NetSyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::OnlyServerWait))
	{
		NetSyncTask->OnSync.AddDynamic(this, &ThisClass::OnNetSync);
		NetSyncTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Interact_Active::OnNetSync()
{
	if (TriggerInteraction())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

bool UD1GameplayAbility_Interact_Active::TriggerInteraction()
{
	bool bTriggerSuccessful = false;
	bool bCanActivate = false;
	
	FGameplayEventData Payload;
	Payload.EventTag = D1GameplayTags::Ability_Interact;
	Payload.Instigator = GetAvatarActorFromActorInfo();
	Payload.Target = InteractableActor;
	
	Interactable->CustomizeInteractionEventData(D1GameplayTags::Ability_Interact, Payload);
	
	if (UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo())
	{
		if (FGameplayAbilitySpec* AbilitySpec = AbilitySystem->FindAbilitySpecFromClass(InteractionInfo.AbilityToGrant))
		{
			bCanActivate = AbilitySpec->Ability->CanActivateAbility(AbilitySpec->Handle, AbilitySystem->AbilityActorInfo.Get());
			bTriggerSuccessful = AbilitySystem->TriggerAbilityFromGameplayEvent(
				AbilitySpec->Handle,
				AbilitySystem->AbilityActorInfo.Get(),
				D1GameplayTags::Ability_Interact,
				&Payload,
				*AbilitySystem
			);
		}
	}

	return bCanActivate || bTriggerSuccessful;
}
