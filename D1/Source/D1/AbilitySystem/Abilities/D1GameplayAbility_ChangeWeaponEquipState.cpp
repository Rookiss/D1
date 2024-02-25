#include "D1GameplayAbility_ChangeWeaponEquipState.h"

#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/D1PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_ChangeWeaponEquipState)

UD1GameplayAbility_ChangeWeaponEquipState::UD1GameplayAbility_ChangeWeaponEquipState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_ChangeWeaponEquipState::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData)
	{
		EWeaponEquipState WeaponEquipState = (EWeaponEquipState)UKismetMathLibrary::Round(TriggerEventData->EventMagnitude);

		UD1EquipmentManagerComponent* EquipmentManager = GetPlayerController()->EquipmentManagerComponent;
		EquipmentManager->Server_RequestChangeWeaponEquipState(WeaponEquipState);
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}
