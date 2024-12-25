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
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Item/Managers/D1InventoryManagerComponent.h"
#include "Item/Managers/D1ItemManagerComponent.h"
#include "Player/LyraPlayerController.h"
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
	
	SnapshottedAttackRate = DefaultAttackRate;
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (const UD1CombatSet* CombatSet = Cast<UD1CombatSet>(ASC->GetAttributeSet(UD1CombatSet::StaticClass())))
		{
			float AttackSpeedPercent = CombatSet->GetAttackSpeedPercent();
			SnapshottedAttackRate = DefaultAttackRate + (DefaultAttackRate * (AttackSpeedPercent / 100.f));
		}
	}

	ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo();
	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	if (LyraPlayerController && LyraCharacter)
	{
		if (UD1ItemManagerComponent* ItemManager = LyraPlayerController->FindComponentByClass<UD1ItemManagerComponent>())
		{
			UD1InventoryManagerComponent* InventoryManager = LyraCharacter->FindComponentByClass<UD1InventoryManagerComponent>();
			ItemManager->RemoveAllowedComponent(InventoryManager);
			
			UD1EquipmentManagerComponent* EquipmentManager = LyraCharacter->FindComponentByClass<UD1EquipmentManagerComponent>();
			ItemManager->RemoveAllowedComponent(EquipmentManager);
		}
	}
}

void UD1GameplayAbility_Equipment::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo();
	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	if (LyraPlayerController && LyraCharacter)
	{
		if (UD1ItemManagerComponent* ItemManager = LyraPlayerController->FindComponentByClass<UD1ItemManagerComponent>())
		{
			UD1InventoryManagerComponent* InventoryManager = LyraCharacter->FindComponentByClass<UD1InventoryManagerComponent>();
			ItemManager->AddAllowedComponent(InventoryManager);
			
			UD1EquipmentManagerComponent* EquipmentManager = LyraCharacter->FindComponentByClass<UD1EquipmentManagerComponent>();
			ItemManager->AddAllowedComponent(EquipmentManager);
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
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
	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	if (LyraCharacter == nullptr)
		return nullptr;
	
	UD1EquipManagerComponent* EquipManager = LyraCharacter->FindComponentByClass<UD1EquipManagerComponent>();
	if (EquipManager == nullptr)
		return nullptr;

	if (EquipmentInfos.Num() > 0)
	{
		for (const FD1EquipmentInfo& EquipmentInfo : EquipmentInfos)
		{
			if (EquipmentInfo.EquipmentType == EEquipmentType::Armor)
				continue;

			if (AD1EquipmentBase* EquipmentActor = EquipManager->GetEquippedActor(EquipmentInfo.WeaponHandType))
				return EquipmentActor;
		}
		return nullptr;
	}
	else
	{
		return EquipManager->GetFirstEquippedActor();
	}
}

UD1ItemInstance* UD1GameplayAbility_Equipment::GetEquipmentItemInstance(const AD1EquipmentBase* InEquipmentActor) const
{
	if (InEquipmentActor == nullptr)
		return nullptr;

	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	if (LyraCharacter == nullptr)
		return nullptr;
	
	UD1EquipManagerComponent* EquipManager = LyraCharacter->FindComponentByClass<UD1EquipManagerComponent>();
	if (EquipManager == nullptr)
		return nullptr;

	return EquipManager->GetEquippedItemInstance(InEquipmentActor->GetEquipmentSlotType());
}

int32 UD1GameplayAbility_Equipment::GetEquipmentStatValue(FGameplayTag InStatTag, const AD1EquipmentBase* InEquipmentActor) const
{
	if (InStatTag.IsValid() && InEquipmentActor)
	{
		if (UD1ItemInstance* ItemInstance = GetEquipmentItemInstance(InEquipmentActor))
		{
			return ItemInstance->GetStatCountByTag(InStatTag);
		}
	}
	return 0;
}
