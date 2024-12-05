#include "D1GameplayAbility_Equipment.h"

#include "AbilitySystemGlobals.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Actors/D1EquipmentBase.h"
#include "Character/LyraCharacter.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equipable_Utility.h"
#include "Item/Fragments/D1ItemFragment_Equipable_Weapon.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Equipment)

UD1GameplayAbility_Equipment::UD1GameplayAbility_Equipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bServerRespectsRemoteAbilityCancellation = false;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;
}

void UD1GameplayAbility_Equipment::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(ActorInfo->AvatarActor.Get());
	if (LyraCharacter == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	UD1EquipManagerComponent* EquipManager = LyraCharacter->FindComponentByClass<UD1EquipManagerComponent>();
	if (EquipManager == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	UD1ItemInstance* ItemInstance = nullptr;
	
	for (FD1EquipmentInfo& EquipmentInfo : EquipmentInfos)
	{
		switch (EquipmentInfo.EquipmentType)
		{
		case EEquipmentType::Armor:
			ItemInstance = EquipManager->GetEquippedItemInstance(EquipmentInfo.RequiredArmorType);
			break;
		case EEquipmentType::Weapon:
			ItemInstance = EquipManager->GetEquippedItemInstance(EquipmentInfo.WeaponHandType);
			EquipmentInfo.EquipmentActor = EquipManager->GetEquippedActor(EquipmentInfo.WeaponHandType);
			break;
		case EEquipmentType::Utility:
			ItemInstance = EquipManager->GetFirstEquippedItemInstance();
			EquipmentInfo.EquipmentActor = EquipManager->GetFirstEquippedActor();
			break;
		}
		
		if ((ItemInstance == nullptr) || (EquipmentInfo.EquipmentType != EEquipmentType::Armor && EquipmentInfo.EquipmentActor == nullptr))
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

bool UD1GameplayAbility_Equipment::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) == false)
		return false;

	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(ActorInfo->AvatarActor.Get());
	if (LyraCharacter == nullptr)
		return false;

	UD1EquipManagerComponent* EquipManager = LyraCharacter->FindComponentByClass<UD1EquipManagerComponent>();
	if (EquipManager == nullptr)
		return false;
	
	for (const FD1EquipmentInfo& EquipmentInfo : EquipmentInfos)
	{
		if (EquipmentInfo.EquipmentType == EEquipmentType::Count)
			return false;

		if (EquipmentInfo.EquipmentType == EEquipmentType::Armor)
		{
			UD1ItemInstance* ItemInstance = EquipManager->GetEquippedItemInstance(EquipmentInfo.RequiredArmorType);
			if (ItemInstance == nullptr)
				return false;
		}
		else if (EquipmentInfo.EquipmentType == EEquipmentType::Weapon)
		{
			UD1ItemInstance* ItemInstance = EquipManager->GetEquippedItemInstance(EquipmentInfo.WeaponHandType);
			if (ItemInstance == nullptr)
				return false;

			const UD1ItemFragment_Equipable_Weapon* WeaponFragment = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equipable_Weapon>();
			if (WeaponFragment == nullptr)
				return false;
	
			if (WeaponFragment->WeaponType != EquipmentInfo.RequiredWeaponType)
				return false;
		}
		else if (EquipmentInfo.EquipmentType == EEquipmentType::Utility)
		{
			UD1ItemInstance* ItemInstance = EquipManager->GetFirstEquippedItemInstance();
			if (ItemInstance == nullptr)
				return false;

			const UD1ItemFragment_Equipable_Utility* UtilityFragment = ItemInstance->FindFragmentByClass<UD1ItemFragment_Equipable_Utility>();
			if (UtilityFragment == nullptr)
				return false;

			if (UtilityFragment->UtilityType != EquipmentInfo.RequiredUtilityType)
				return false;
		}
	}

	return true;
}

AD1EquipmentBase* UD1GameplayAbility_Equipment::GetFirstEquipmentActor() const
{
	for (const FD1EquipmentInfo& EquipmentInfo : EquipmentInfos)
	{
		if (AD1EquipmentBase* EquipmentActor = EquipmentInfo.EquipmentActor.Get())
			return EquipmentActor;
	}
	return nullptr;
}

UD1ItemInstance* UD1GameplayAbility_Equipment::GetEquipmentItemInstance(const AD1EquipmentBase* InEquipmentActor) const
{
	if (InEquipmentActor == nullptr)
		return nullptr;
	
	UD1EquipManagerComponent* EquipManager = GetLyraCharacterFromActorInfo()->FindComponentByClass<UD1EquipManagerComponent>();
	if (EquipManager == nullptr)
		return nullptr;
	
	for (const FD1EquipmentInfo& EquipmentInfo : EquipmentInfos)
	{
		if (EquipmentInfo.EquipmentActor != InEquipmentActor)
			continue;
		
		return EquipManager->GetEquippedItemInstance(InEquipmentActor->GetEquipmentSlotType());
	}

	return nullptr;
}

int32 UD1GameplayAbility_Equipment::GetEquipmentStatValue(FGameplayTag InStatTag, const AD1EquipmentBase* InEquipmentActor) const
{
	if (InStatTag.IsValid() == false || InEquipmentActor == nullptr)
		return 0;
	
	if (UD1ItemInstance* ItemInstance = GetEquipmentItemInstance(InEquipmentActor))
		return ItemInstance->GetStackCountByTag(InStatTag);

	check(true);
	
	return 0;
}
