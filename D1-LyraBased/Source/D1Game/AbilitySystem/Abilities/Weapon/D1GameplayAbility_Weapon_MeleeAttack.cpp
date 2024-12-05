#include "D1GameplayAbility_Weapon_MeleeAttack.h"

#include "D1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Actors/D1EquipmentBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon_MeleeAttack)

UD1GameplayAbility_Weapon_MeleeAttack::UD1GameplayAbility_Weapon_MeleeAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationGroup = ELyraAbilityActivationGroup::Exclusive_Replaceable;
}

void UD1GameplayAbility_Weapon_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bBlocked = false;

	if (UAbilityTask_WaitGameplayEvent* GameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, D1GameplayTags::GameplayEvent_Trace, nullptr, false, true))
    {
    	GameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnTargetDataReady);
    	GameplayEventTask->ReadyForActivation();
    }

    if (UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("MeleeAttack"), AttackMontage, GetSnapshottedAttackRate(), NAME_None, false, 1.f, 0.f, false))
    {
    	PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
    	PlayMontageTask->ReadyForActivation();
    }

	if (UAbilityTask_WaitGameplayEvent* GameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, D1GameplayTags::GameplayEvent_Montage_End, nullptr, true, true))
	{
		GameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnMontageEventTriggered);
		GameplayEventTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Weapon_MeleeAttack::OnTargetDataReady(FGameplayEventData Payload)
{
	if (bBlocked)
		return;

	AD1EquipmentBase* WeaponActor = const_cast<AD1EquipmentBase*>(Cast<AD1EquipmentBase>(Payload.Instigator));
	if (WeaponActor == nullptr)
		return;
	
	ULyraAbilitySystemComponent* SourceASC = GetLyraAbilitySystemComponentFromActorInfo();
	if (SourceASC == nullptr)
		return;
	
	if (SourceASC->FindAbilitySpecFromHandle(CurrentSpecHandle))
	{
		FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(Payload.TargetData)));

		TArray<int32> CharacterHitIndexes;
		TArray<int32> BlockHitIndexes;
		ParseTargetData(LocalTargetDataHandle, CharacterHitIndexes, BlockHitIndexes);

		float Damage = GetEquipmentStatValue(D1GameplayTags::SetByCaller_BaseDamage, WeaponActor);
		
		if (BlockHitIndexes.Num() > 0)
		{
			FHitResult HitResult = *(LocalTargetDataHandle.Data[BlockHitIndexes[0]]->GetHitResult());
			ProcessHitResult(HitResult, Damage, true, BackwardMontage, WeaponActor);
			bBlocked = true;
		}
		else
		{
			for (int32 CharqacterHitIndex : CharacterHitIndexes)
			{
				FHitResult HitResult = *LocalTargetDataHandle.Data[CharqacterHitIndex]->GetHitResult();
				ProcessHitResult(HitResult, Damage, false, nullptr, WeaponActor);
			}
		}
	}
}

void UD1GameplayAbility_Weapon_MeleeAttack::OnMontageEventTriggered(FGameplayEventData Payload)
{
	HandleMontageEvent(Payload);
}

void UD1GameplayAbility_Weapon_MeleeAttack::HandleMontageEvent(FGameplayEventData Payload)
{
	OnMontageFinished();
}

void UD1GameplayAbility_Weapon_MeleeAttack::OnMontageFinished()
{
	if (HasAuthority(&CurrentActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
