#include "D1GameplayAbility_ChangeWeaponEquipState.h"

#include "D1GameplayTags.h"
#include "Character/D1Player.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Player/D1PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_ChangeWeaponEquipState)

UD1GameplayAbility_ChangeWeaponEquipState::UD1GameplayAbility_ChangeWeaponEquipState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_ChangeWeaponEquipState::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayAbilitySpec* AbilitySpec = GetCurrentAbilitySpec();
	check(AbilitySpec);
	
	AD1PlayerController* PC = Cast<AD1PlayerController>(GetPlayerController());
	check(PC);
	
	EquipmentManager = PC->EquipmentManagerComponent;
	check(EquipmentManager);
	
	const FGameplayTag& EventTag = TriggerEventData->EventTag;

	EWeaponEquipState NewWeaponEquipState = EWeaponEquipState::Count;
	
	if (EventTag == D1GameplayTags::Event_EquipWeapon_ToggleArming)
	{
		// Unarmed -> Armed
		if (EquipmentManager->GetCurrWeaponEquipState() == EWeaponEquipState::Unarmed)
		{
			if (EquipmentManager->CanChangeWeaponEquipState(EquipmentManager->GetPrevWeaponEquipState()))
			{
				NewWeaponEquipState = EquipmentManager->GetPrevWeaponEquipState();
			}
			else
			{
				// Invalid PrevEquipType -> Find Valid EquipType (Along Armed Types)
				for (int32 i = 1; i < (int32)EWeaponEquipState::Count; i++)
				{
					if ((EWeaponEquipState)i == EquipmentManager->GetPrevWeaponEquipState())
						continue;

					if (EquipmentManager->CanChangeWeaponEquipState((EWeaponEquipState)i))
					{
						NewWeaponEquipState = (EWeaponEquipState)i;
						break;
					}
				}
			}
		}
		// Armed -> Unarmed
		else
		{
			NewWeaponEquipState = EWeaponEquipState::Unarmed;
		}
	}
	else if (EventTag == D1GameplayTags::Event_EquipWeapon_Primary)
	{
		NewWeaponEquipState = EWeaponEquipState::Primary;
	}
	else if (EventTag == D1GameplayTags::Event_EquipWeapon_Secondary)
	{
		NewWeaponEquipState = EWeaponEquipState::Secondary;
	}
	else if (EventTag == D1GameplayTags::Event_EquipWeapon_CycleBackward)
	{
		NewWeaponEquipState = EquipmentManager->GetBackwardWeaponEquipState(EquipmentManager->GetCurrWeaponEquipState());
	}
	else if (EventTag == D1GameplayTags::Event_EquipWeapon_CycleForward)
	{
		NewWeaponEquipState = EquipmentManager->GetForwardWeaponEquipState(EquipmentManager->GetCurrWeaponEquipState());
	}

	if (EquipmentManager->CanChangeWeaponEquipState(NewWeaponEquipState))
	{
		ChangeWeaponEquipState(NewWeaponEquipState);
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateInputDirectly, true);
	}
}
