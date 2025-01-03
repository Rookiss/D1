#include "D1GameplayAbility_Bow_Reload.h"

#include "D1GameplayTags.h"
#include "AbilitySystem/Abilities/LyraAbilityCost_InventoryItem.h"
#include "Item/Fragments/D1ItemFragment_Equipable_Weapon.h"
#include "Item/Managers/D1InventoryManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Bow_Reload)

UD1GameplayAbility_Bow_Reload::UD1GameplayAbility_Bow_Reload(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1GameplayAbility_Bow_Reload::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if (UD1ItemFragment_Equipable_Weapon* WeaponFragment = Cast<UD1ItemFragment_Equipable_Weapon>(Spec.SourceObject.Get()))
	{
		for (ULyraAbilityCost* LyraAbilityCost : AdditionalCosts)
		{
			if (ULyraAbilityCost_InventoryItem* InventoryItemCost = Cast<ULyraAbilityCost_InventoryItem>(LyraAbilityCost))
			{
				InventoryItemCost->ItemTemplateClass = WeaponFragment->AmmoItemTemplateClass;
				break;
			}
		}
	}
	
	Super::OnGiveAbility(ActorInfo, Spec);
}

void UD1GameplayAbility_Bow_Reload::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (K2_CheckAbilityCost() == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
}
