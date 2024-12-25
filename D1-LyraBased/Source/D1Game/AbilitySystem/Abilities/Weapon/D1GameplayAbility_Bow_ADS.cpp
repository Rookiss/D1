#include "D1GameplayAbility_Bow_ADS.h"

#include "D1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Item/Managers/D1InventoryManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Bow_ADS)

UD1GameplayAbility_Bow_ADS::UD1GameplayAbility_Bow_ADS(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    AbilityTags.AddTag(D1GameplayTags::Ability_ADS_Bow);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_ADS_Bow);
	ActivationBlockedTags.AddTag(D1GameplayTags::Status_Attack);
}

void UD1GameplayAbility_Bow_ADS::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	SetCameraMode(CameraModeClass);

	InputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);
	if (InputReleaseTask)
	{
		InputReleaseTask->OnRelease.AddDynamic(this, &ThisClass::OnInputRelease);
		InputReleaseTask->ReadyForActivation();
	}

	ADSEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, D1GameplayTags::GameplayEvent_Bow_ADS, nullptr, false, true);
	if (ADSEventTask)
	{
		ADSEventTask->EventReceived.AddDynamic(this, &ThisClass::OnADSEvent);
		ADSEventTask->ReadyForActivation();
	}

	StartADS();
}

void UD1GameplayAbility_Bow_ADS::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ResetADS();

	if (bWasCancelled)
	{
		if (UAbilityTask_PlayMontageAndWait* ADSEndMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ADSEndMontage"), ADSEndMontage, 1.f, NAME_None, false))
		{
			ADSEndMontageTask->ReadyForActivation();
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_Bow_ADS::OnADSEvent(FGameplayEventData Payload)
{
	if (Payload.EventMagnitude == 0.f)
	{
		K2_CancelAbility();
	}
	else if (Payload.EventMagnitude == 1.f)
	{
		SendGameplayEvent(D1GameplayTags::Ability_Reload_Bow, FGameplayEventData());
	}
	else if (Payload.EventMagnitude == 2.f)
	{
		StartADS();
	}
}

void UD1GameplayAbility_Bow_ADS::OnADSStartBegin(FGameplayEventData Payload)
{
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(D1GameplayTags::Status_ADS_Ready);
	UAbilitySystemBlueprintLibrary::AddLooseGameplayTags(GetAvatarActorFromActorInfo(), TagContainer, true);
}

void UD1GameplayAbility_Bow_ADS::OnInputRelease(float TimeHeld)
{
	ResetADS();

	CheckEndADS();

	if (UAbilityTask_WaitGameplayTagRemoved* WaitAttackEndTask = UAbilityTask_WaitGameplayTagRemoved::WaitGameplayTagRemove(this, D1GameplayTags::Status_Attack, nullptr, true))
	{
		WaitAttackEndTask->Removed.AddDynamic(this, &ThisClass::CheckEndADS);
		WaitAttackEndTask->ReadyForActivation();
	}

	if (UAbilityTask_WaitGameplayTagRemoved* WaitReloadEndTask = UAbilityTask_WaitGameplayTagRemoved::WaitGameplayTagRemove(this, D1GameplayTags::Status_Reload, nullptr, true))
	{
		WaitReloadEndTask->Removed.AddDynamic(this, &ThisClass::CheckEndADS);
		WaitReloadEndTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Bow_ADS::CheckEndADS()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC == nullptr || (ASC->HasMatchingGameplayTag(D1GameplayTags::Status_Attack) == false && ASC->HasMatchingGameplayTag(D1GameplayTags::Status_Reload) == false))
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UD1GameplayAbility_Bow_ADS::StartADS()
{
	if (UAbilityTask_PlayMontageAndWait* ADSStartMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ADSStartMontage"), ADSStartMontage, 1.f, NAME_None, true))
	{
		ADSStartMontageTask->ReadyForActivation();
	}
	
	ADSStartBeginEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, D1GameplayTags::GameplayEvent_Montage_Begin, nullptr, true, true);
	if (ADSStartBeginEventTask)
	{
		ADSStartBeginEventTask->EventReceived.AddDynamic(this, &ThisClass::OnADSStartBegin);
		ADSStartBeginEventTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Bow_ADS::ResetADS()
{
	if (ADSEventTask)
	{
		ADSEventTask->EndTask();
	}

	if (InputReleaseTask)
	{
		InputReleaseTask->EndTask();
	}

	if (ADSStartBeginEventTask)
	{
		ADSStartBeginEventTask->EndTask();
	}

	ClearCameraMode();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC && ASC->HasMatchingGameplayTag(D1GameplayTags::Status_ADS_Ready))
	{
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(D1GameplayTags::Status_ADS_Ready);
		UAbilitySystemBlueprintLibrary::RemoveLooseGameplayTags(GetAvatarActorFromActorInfo(), TagContainer, true);
	}
}
