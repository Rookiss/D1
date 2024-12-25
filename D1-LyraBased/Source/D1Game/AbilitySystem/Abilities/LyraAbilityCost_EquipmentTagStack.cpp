#include "LyraAbilityCost_EquipmentTagStack.h"

#include "D1GameplayTags.h"
#include "Gladiator/D1GameplayAbility_Equipment.h"
#include "Item/D1ItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraAbilityCost_EquipmentTagStack)

ULyraAbilityCost_EquipmentTagStack::ULyraAbilityCost_EquipmentTagStack()
{
	Quantity.SetValue(1.f);
}

bool ULyraAbilityCost_EquipmentTagStack::CheckCost(const ULyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (const UD1GameplayAbility_Equipment* EquipmentAbility = Cast<const UD1GameplayAbility_Equipment>(Ability))
	{
		if (UD1ItemInstance* ItemInstance = EquipmentAbility->GetEquipmentItemInstance(EquipmentAbility->GetFirstEquipmentActor()))
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

			const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumStacks = FMath::TruncToInt(NumStacksReal);
			const bool bCanApplyCost = ItemInstance->GetOwnedCountByTag(Tag) >= NumStacks;
			
			if (bCanApplyCost == false && OptionalRelevantTags && FailureTag.IsValid())
			{
				OptionalRelevantTags->AddTag(FailureTag);				
			}
			return bCanApplyCost;
		}
	}
	return false;
}

void ULyraAbilityCost_EquipmentTagStack::ApplyCost(const ULyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo->IsNetAuthority())
	{
		if (const UD1GameplayAbility_Equipment* EquipmentAbility = Cast<const UD1GameplayAbility_Equipment>(Ability))
		{
			if (UD1ItemInstance* ItemInstance = EquipmentAbility->GetEquipmentItemInstance(EquipmentAbility->GetFirstEquipmentActor()))
			{
				const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

				const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
				const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

				ItemInstance->AddOrRemoveOwnedTagStack(Tag, -NumStacks);
			}
		}
	}
}
