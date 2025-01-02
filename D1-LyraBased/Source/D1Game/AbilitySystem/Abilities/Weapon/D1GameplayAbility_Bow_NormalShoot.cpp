#include "D1GameplayAbility_Bow_NormalShoot.h"

#include "D1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Actors/D1EquipmentBase.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Bow_NormalShoot)

UD1GameplayAbility_Bow_NormalShoot::UD1GameplayAbility_Bow_NormalShoot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bServerRespectsRemoteAbilityCancellation = true;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	
    AbilityTags.AddTag(D1GameplayTags::Ability_Attack_Bow);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_Attack);
	ActivationRequiredTags.AddTag(D1GameplayTags::Status_ADS_Ready);
	ActivationBlockedTags.AddTag(D1GameplayTags::Status_PiercingShot);
}

void UD1GameplayAbility_Bow_NormalShoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (UAbilityTask_PlayMontageAndWait* ShootMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ShootMontage"), ReleaseMontage, GetSnapshottedAttackRate(), NAME_None, true, 1.f, 0.f, false))
	{
		ShootMontageTask->ReadyForActivation();
	}

	if (UAbilityTask_WaitGameplayEvent* GameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, D1GameplayTags::GameplayEvent_Montage_End, nullptr, true, true))
	{
		GameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnMontageEnded);
		GameplayEventTask->ReadyForActivation();
	}
	
	if (HasAuthority(&CurrentActivationInfo))
	{
		SpawnProjectile();
	}

	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(D1GameplayTags::Status_ADS_Ready);
	UAbilitySystemBlueprintLibrary::RemoveLooseGameplayTags(GetAvatarActorFromActorInfo(), TagContainer, true);
}

void UD1GameplayAbility_Bow_NormalShoot::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	NotifyToADS(bWasCancelled);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_Bow_NormalShoot::OnMontageEnded(FGameplayEventData Payload)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UD1GameplayAbility_Bow_NormalShoot::NotifyToADS(bool bShouldCancel)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayEventData Payload;
		Payload.EventMagnitude = bShouldCancel ? 0.f : 1.f;
		ASC->HandleGameplayEvent(D1GameplayTags::GameplayEvent_Bow_ADS, &Payload);
	}
}
