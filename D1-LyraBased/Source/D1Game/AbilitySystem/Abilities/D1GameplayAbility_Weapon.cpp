#include "D1GameplayAbility_Weapon.h"

#include "AbilitySystemGlobals.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraCharacter.h"
#include "Item/D1ItemInstance.h"
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
	WeaponActor = nullptr;
	
	if (ALyraCharacter* PlayerCharacter = Cast<ALyraCharacter>(ActorInfo->AvatarActor.Get()))
	{
		if (UD1EquipManagerComponent* EquipManager = PlayerCharacter->FindComponentByClass<UD1EquipManagerComponent>())
		{
			WeaponActor = EquipManager->GetEquippedActor(WeaponHandType);
		}
	}
	
	if (WeaponActor == nullptr)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

int32 UD1GameplayAbility_Weapon::GetWeaponStatValue(FGameplayTag StatTag) const
{
	int32 StatValue = 0;
	if (UD1EquipManagerComponent* EquipManager = GetLyraCharacterFromActorInfo()->FindComponentByClass<UD1EquipManagerComponent>())
	{
		if (const UD1ItemInstance* ItemInstance = EquipManager->GetEquippedItemInstance(WeaponHandType))
		{
			StatValue = ItemInstance->GetStackCountByTag(StatTag);
		}
	}
	return StatValue;
}

float UD1GameplayAbility_Weapon::GetAttackRate() const
{
	float AttackRate = DefaultAttackRate;
	
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (const UD1CombatSet* CombatSet = Cast<UD1CombatSet>(ASC->GetAttributeSet(UD1CombatSet::StaticClass())))
		{
			float AttackSpeedPercent = CombatSet->GetAttackSpeedPercent();
			AttackRate = DefaultAttackRate + (DefaultAttackRate * (AttackSpeedPercent / 100.f));
		}
	}
	
	return AttackRate;
}
