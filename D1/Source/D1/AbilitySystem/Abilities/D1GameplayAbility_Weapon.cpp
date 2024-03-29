#include "D1GameplayAbility_Weapon.h"

#include "D1Define.h"
#include "D1GameplayTags.h"
#include "Character/D1Player.h"
#include "Item/Managers/D1EquipManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Weapon)

UD1GameplayAbility_Weapon::UD1GameplayAbility_Weapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Weapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (AD1Player* PlayerCharacter = Cast<AD1Player>(ActorInfo->AvatarActor.Get()))
	{
		UD1EquipManagerComponent* EquipManager = PlayerCharacter->EquipManagerComponent;
		EWeaponEquipState WeaponEquipState = EquipManager->GetCurrentWeaponEquipState();
		const TArray<FD1EquipEntry>& Entries = EquipManager->GetAllEntries();
	
		const FGameplayAbilitySpec* AbilitySpec = GetCurrentAbilitySpec();
		
		if (AbilitySpec->DynamicAbilityTags.HasTagExact(D1GameplayTags::Input_Action_Attack_MainHand))
		{
			switch (WeaponEquipState)
			{
			case EWeaponEquipState::Unarmed:
				WeaponActor = Entries[(int32)EEquipmentSlotType::Unarmed_LeftHand].SpawnedWeaponActor;
				break;
			case EWeaponEquipState::Primary:
				WeaponActor = Entries[(int32)EEquipmentSlotType::Primary_RightHand].SpawnedWeaponActor;
				if (WeaponActor == nullptr)
				{
					WeaponActor = Entries[(int32)EEquipmentSlotType::Primary_TwoHand].SpawnedWeaponActor;
				}
				break;
			case EWeaponEquipState::Secondary:
				WeaponActor = Entries[(int32)EEquipmentSlotType::Secondary_RightHand].SpawnedWeaponActor;
				if (WeaponActor == nullptr)
				{
					WeaponActor = Entries[(int32)EEquipmentSlotType::Secondary_TwoHand].SpawnedWeaponActor;
				}
				break;
			}
		}
		else if (AbilitySpec->DynamicAbilityTags.HasTagExact(D1GameplayTags::Input_Action_Attack_OffHand))
		{
			switch (WeaponEquipState)
			{
			case EWeaponEquipState::Unarmed:
				WeaponActor = Entries[(int32)EEquipmentSlotType::Unarmed_RightHand].SpawnedWeaponActor;
				break;
			case EWeaponEquipState::Primary:
				WeaponActor = Entries[(int32)EEquipmentSlotType::Primary_LeftHand].SpawnedWeaponActor;
				if (WeaponActor == nullptr)
				{
					WeaponActor = Entries[(int32)EEquipmentSlotType::Primary_TwoHand].SpawnedWeaponActor;
				}
				break;
			case EWeaponEquipState::Secondary:
				WeaponActor = Entries[(int32)EEquipmentSlotType::Secondary_LeftHand].SpawnedWeaponActor;
				if (WeaponActor == nullptr)
				{
					WeaponActor = Entries[(int32)EEquipmentSlotType::Secondary_TwoHand].SpawnedWeaponActor;
				}
				break;
			}
		}
	}
	
	if (WeaponActor == nullptr)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
