#include "D1GameplayAbility_Interact_Object.h"

#include "D1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Actors/D1EquipmentBase.h"
#include "Character/LyraCharacter.h"
#include "Interaction/D1Interactable.h"
#include "Interaction/D1WorldInteractable.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Tasks/D1AbilityTask_WaitForInvalidInteraction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Interact_Object)

UD1GameplayAbility_Interact_Object::UD1GameplayAbility_Interact_Object(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    ActivationPolicy = ELyraAbilityActivationPolicy::Manual;

	AbilityTags.AddTag(D1GameplayTags::Ability_Interact_Object);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_Interact);
}

void UD1GameplayAbility_Interact_Object::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	AActor* TargetActor = const_cast<AActor*>(TriggerEventData->Target.Get());
	if (InitializeAbility(TargetActor) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	bInitialized = false;

	FD1InteractionQuery Query;
	Query.RequestingAvatar = GetAvatarActorFromActorInfo();
	Query.RequestingController = GetControllerFromActorInfo();

	if (Interactable->CanInteraction(Query) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	bInitialized = true;

	if (AD1WorldInteractable* WorldInteractable = Cast<AD1WorldInteractable>(TargetActor))
	{
		WorldInteractable->OnInteractionSuccess(GetAvatarActorFromActorInfo());
	}
	
	if (UD1AbilityTask_WaitForInvalidInteraction* InvalidInteractionTask = UD1AbilityTask_WaitForInvalidInteraction::WaitForInvalidInteraction(this, AcceptanceAngle, AcceptanceDistance))
	{
		InvalidInteractionTask->OnInvalidInteraction.AddDynamic(this, &ThisClass::OnInvalidInteraction);
		InvalidInteractionTask->ReadyForActivation();
	}

	if (UAnimMontage* ActiveEndMontage = InteractionInfo.ActiveEndMontage)
	{
		if (UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("InteractMontage"), ActiveEndMontage, 1.f, NAME_None, true, 1.f, 0.f, false))
		{
			PlayMontageTask->ReadyForActivation();
		}
	}
}

void UD1GameplayAbility_Interact_Object::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (InteractionInfo.Duration > 0.f)
	{
		if (ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo())
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
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_Interact_Object::OnInvalidInteraction()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}
