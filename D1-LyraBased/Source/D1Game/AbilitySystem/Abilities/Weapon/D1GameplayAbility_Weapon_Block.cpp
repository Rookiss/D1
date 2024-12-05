#include "D1GameplayAbility_Weapon_Block.h"

#include "D1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Actors/D1EquipmentBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_Block)

UD1GameplayAbility_Weapon_Block::UD1GameplayAbility_Weapon_Block(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	AbilityTags.AddTag(D1GameplayTags::Ability_Block);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_Block);
}

void UD1GameplayAbility_Weapon_Block::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UAbilityTask_PlayMontageAndWait* BlockStartMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("BlockStartMontage"), BlockStartMontage, 1.f, NAME_None, true))
	{
		BlockStartMontageTask->ReadyForActivation();
	}

	if (HasAuthority(&CurrentActivationInfo))
	{
		if (AD1EquipmentBase* WeaponActor = GetFirstEquipmentActor())
		{
			WeaponActor->ChangeBlockState(true);
		}
	}
	
	if (UAbilityTask_WaitInputRelease* InputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true))
	{
		InputReleaseTask->OnRelease.AddDynamic(this, &ThisClass::OnInputRelease);
		InputReleaseTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Weapon_Block::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (AD1EquipmentBase* WeaponActor = GetFirstEquipmentActor())
	{
		WeaponActor->ChangeBlockState(false);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_Weapon_Block::OnInputRelease(float TimeHeld)
{
	if (UAbilityTask_PlayMontageAndWait* BlockEndMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("BlockEndMontage"), BlockEndMontage, 1.f, NAME_None, true))
	{
		BlockEndMontageTask->ReadyForActivation();
	}

	if (HasAuthority(&CurrentActivationInfo))
	{
		if (AD1EquipmentBase* WeaponActor = GetFirstEquipmentActor())
		{
			WeaponActor->ChangeBlockState(false);
		}

		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
