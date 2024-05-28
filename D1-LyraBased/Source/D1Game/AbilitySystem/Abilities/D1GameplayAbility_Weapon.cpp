#include "D1GameplayAbility_Weapon.h"

#include "Character/LyraCharacter.h"
#include "Item/Managers/D1EquipManagerComponent.h"

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
			WeaponActor = EquipManager->GetEquippedWeapon(WeaponHandType);
		}
	}
	
	if (WeaponActor == nullptr)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
