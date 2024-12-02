#include "D1GameplayAbility_Weapon.h"

#include "AbilitySystemGlobals.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Actors/D1WeaponBase.h"
#include "Character/LyraCharacter.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equipable_Weapon.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon)

UD1GameplayAbility_Weapon::UD1GameplayAbility_Weapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bServerRespectsRemoteAbilityCancellation = false;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;
}

void UD1GameplayAbility_Weapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ALyraCharacter* SourceCharacter = Cast<ALyraCharacter>(ActorInfo->AvatarActor.Get());
	if (SourceCharacter == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	UD1EquipManagerComponent* EquipManager = SourceCharacter->FindComponentByClass<UD1EquipManagerComponent>();
	if (EquipManager == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	for (FD1WeaponInfo& WeaponInfo : WeaponInfos)
	{
		WeaponInfo.WeaponActor = EquipManager->GetEquippedActor(WeaponInfo.WeaponHandType);
		if (WeaponInfo.WeaponActor == nullptr)
		{
			CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
			return;
		}
	}
	
	SnapshottedAttackRate = DefaultAttackRate;
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (const UD1CombatSet* CombatSet = Cast<UD1CombatSet>(ASC->GetAttributeSet(UD1CombatSet::StaticClass())))
		{
			float AttackSpeedPercent = CombatSet->GetAttackSpeedPercent();
			SnapshottedAttackRate = DefaultAttackRate + (DefaultAttackRate * (AttackSpeedPercent / 100.f));
		}
	}
}

bool UD1GameplayAbility_Weapon::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) == false)
		return false;

	ALyraCharacter* SourceCharacter = Cast<ALyraCharacter>(ActorInfo->AvatarActor.Get());
	if (SourceCharacter == nullptr)
		return false;

	UD1EquipManagerComponent* EquipManager = SourceCharacter->FindComponentByClass<UD1EquipManagerComponent>();
	if (EquipManager == nullptr)
		return false;
	
	for (const FD1WeaponInfo& WeaponInfo : WeaponInfos)
	{
		if (WeaponInfo.bShouldCheckWeaponType == false)
			continue;

		UD1ItemInstance* ItemInstance = EquipManager->GetEquippedItemInstance(WeaponInfo.WeaponHandType);
		if (ItemInstance == nullptr)
			return false;

		const UD1ItemFragment_Equipable_Weapon* WeaponFragment = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equipable_Weapon>();
		if (WeaponFragment == nullptr)
			return false;
	
		if (WeaponFragment->WeaponType != WeaponInfo.RequiredWeaponType)
			return false;
	}

	return true;
}

AD1WeaponBase* UD1GameplayAbility_Weapon::GetFirstWeaponActor() const
{
	if (WeaponInfos.Num() > 0)
	{
		return WeaponInfos[0].WeaponActor;
	}
	return nullptr;
}

int32 UD1GameplayAbility_Weapon::GetWeaponStatValue(FGameplayTag InStatTag, const AD1WeaponBase* InWeaponActor) const
{
	if (InStatTag.IsValid() == false || InWeaponActor == nullptr)
		return 0;

	int32 StatValue = 0;
	if (UD1EquipManagerComponent* EquipManager = GetLyraCharacterFromActorInfo()->FindComponentByClass<UD1EquipManagerComponent>())
	{
		EWeaponHandType WeaponHandType = UD1EquipManagerComponent::ConvertToWeaponHandType(InWeaponActor->GetEquipmentSlotType());
		if (const UD1ItemInstance* ItemInstance = EquipManager->GetEquippedItemInstance(WeaponHandType))
		{
			return ItemInstance->GetStackCountByTag(InStatTag);
		}
	}
	return StatValue;
}
