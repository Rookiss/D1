#include "D1GameplayAbility_Bow_AutoReload.h"

#include "D1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equipable_Weapon.h"
#include "Item/Managers/D1InventoryManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Bow_AutoReload)

UD1GameplayAbility_Bow_AutoReload::UD1GameplayAbility_Bow_AutoReload(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    ActivationPolicy = ELyraAbilityActivationPolicy::OnSpawn;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;

	TArray<FGameplayTag> TagArray = {
		D1GameplayTags::Status_Reload,
		D1GameplayTags::Status_Attack,
		D1GameplayTags::Status_Interact,
		D1GameplayTags::Status_Skill,
		D1GameplayTags::Status_HitReact,
		D1GameplayTags::Status_Dash,
		D1GameplayTags::Status_Death,
		D1GameplayTags::Status_Knockback,
		D1GameplayTags::Status_Stun
	};
	CheckTagContainer = FGameplayTagContainer::CreateFromArray(TagArray);
}

void UD1GameplayAbility_Bow_AutoReload::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::PeriodicCheck, CheckInterval, true);
}

void UD1GameplayAbility_Bow_AutoReload::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_Bow_AutoReload::PeriodicCheck()
{
	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	if (LyraCharacter && LyraCharacter->HasAnyMatchingGameplayTags(CheckTagContainer) == false)
	{
		if (CheckAmmoState())
		{
			SendGameplayEvent(D1GameplayTags::Ability_Reload_Bow, FGameplayEventData());
		}
	}
}

bool UD1GameplayAbility_Bow_AutoReload::CheckAmmoState()
{
	if (WeaponItemInstance.IsValid() == false || AmmoItemTemplateClass == nullptr)
	{
		WeaponItemInstance = nullptr;
		AmmoItemTemplateClass = nullptr;
		
		WeaponItemInstance = GetEquipmentItemInstance(GetFirstEquipmentActor());
		if (WeaponItemInstance.IsValid() == false)
			return false;

		if (const UD1ItemFragment_Equipable_Weapon* WeaponFragment = WeaponItemInstance->FindFragmentByClass<UD1ItemFragment_Equipable_Weapon>())
		{
			AmmoItemTemplateClass = WeaponFragment->AmmoItemTemplateClass;
		}

		if (AmmoItemTemplateClass == nullptr)
			return false;
	}
	
	if (ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo())
	{
		if (UD1InventoryManagerComponent* InventoryManager = LyraCharacter->GetComponentByClass<UD1InventoryManagerComponent>())
		{
			const int32 LoadedAmmoCount = WeaponItemInstance->GetOwnedCountByTag(D1GameplayTags::Ammo_Arrow);
			const int32 RemainAmmoCount = InventoryManager->GetTotalCountByClass(AmmoItemTemplateClass);
			return (LoadedAmmoCount == 0 && RemainAmmoCount > 0);
		}
	}
	
	return false;
}
