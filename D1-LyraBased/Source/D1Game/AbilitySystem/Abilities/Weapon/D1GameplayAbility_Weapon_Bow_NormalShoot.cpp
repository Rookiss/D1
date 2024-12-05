#include "D1GameplayAbility_Weapon_Bow_NormalShoot.h"

#include "D1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Actors/D1ProjectileBase.h"
#include "Actors/D1EquipmentBase.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_Bow_NormalShoot)

UD1GameplayAbility_Weapon_Bow_NormalShoot::UD1GameplayAbility_Weapon_Bow_NormalShoot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    AbilityTags.AddTag(D1GameplayTags::Ability_Attack_Bow);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_Attack);
	ActivationRequiredTags.AddTag(D1GameplayTags::Status_ADS_Ready);
}

void UD1GameplayAbility_Weapon_Bow_NormalShoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (K2_CheckAbilityCooldown() == false || K2_CheckAbilityCost() == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (HasAuthority(&CurrentActivationInfo))
	{
		SpawnProjectile();
	}

	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(D1GameplayTags::Status_ADS_Ready);
	UAbilitySystemBlueprintLibrary::RemoveLooseGameplayTags(GetAvatarActorFromActorInfo(), TagContainer, true);

	UAnimMontage* SelectedMontage = K2_CheckAbilityCost() ? ReleaseReloadMontage : ReleaseMontage;
	if (UAbilityTask_PlayMontageAndWait* ShootMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ShootMontage"), SelectedMontage, GetSnapshottedAttackRate(), NAME_None, true, 1.f, 0.f, false))
	{
		ShootMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		ShootMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
		ShootMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		ShootMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		ShootMontageTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Weapon_Bow_NormalShoot::OnMontageFinished()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayEventData Payload;
		ASC->HandleGameplayEvent(D1GameplayTags::GameplayEvent_Bow_ADS, &Payload);
	}
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
