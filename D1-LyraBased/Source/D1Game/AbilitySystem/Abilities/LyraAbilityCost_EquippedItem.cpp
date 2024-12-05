#include "LyraAbilityCost_EquippedItem.h"

#include "GameplayAbilitySpec.h"
#include "GameplayAbilitySpecHandle.h"
#include "LyraGameplayAbility.h"
#include "Actors/D1EquipmentBase.h"
#include "Character/LyraCharacter.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraAbilityCost_EquippedItem)

ULyraAbilityCost_EquippedItem::ULyraAbilityCost_EquippedItem()
{
	Quantity.SetValue(1.0f);
}

bool ULyraAbilityCost_EquippedItem::CheckCost(const ULyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (EquipmentType == EEquipmentType::Count)
		return false;

	ALyraCharacter* LyraCharacter = Ability->GetLyraCharacterFromActorInfo();
	if (LyraCharacter == nullptr)
		return false;

	UD1EquipManagerComponent* EquipManager = LyraCharacter->GetComponentByClass<UD1EquipManagerComponent>();
	UD1EquipmentManagerComponent* EquipmentManager = LyraCharacter->GetComponentByClass<UD1EquipmentManagerComponent>();
	if (EquipManager == nullptr || EquipmentManager == nullptr)
		return false;
	
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	switch (EquipmentType)
	{
	case EEquipmentType::Armor:
		EquipmentSlotType = UD1EquipManagerComponent::ConvertToEquipmentSlotType(ArmorType);
		break;
	case EEquipmentType::Weapon:
	case EEquipmentType::Utility:
		EquipmentSlotType = UD1EquipManagerComponent::ConvertToEquipmentSlotType(WeaponHandType, EquipManager->GetCurrentEquipState());
		break;
	}

	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return false;
	
	int32 ItemCount = EquipmentManager->GetItemCount(EquipmentSlotType);
	const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);
	const float NumItemsToConsumeReal = Quantity.GetValueAtLevel(AbilityLevel);
	const int32 NumItemsToConsume = FMath::TruncToInt(NumItemsToConsumeReal);
	
	return ItemCount >= NumItemsToConsume;
}

void ULyraAbilityCost_EquippedItem::ApplyCost(const ULyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo->IsNetAuthority())
	{
		ALyraCharacter* LyraCharacter = Ability->GetLyraCharacterFromActorInfo();
		if (LyraCharacter == nullptr)
			return;

		UD1EquipManagerComponent* EquipManager = LyraCharacter->GetComponentByClass<UD1EquipManagerComponent>();
		UD1EquipmentManagerComponent* EquipmentManager = LyraCharacter->GetComponentByClass<UD1EquipmentManagerComponent>();
		if (EquipManager == nullptr || EquipmentManager == nullptr)
			return;

		EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

		switch (EquipmentType)
		{
		case EEquipmentType::Armor:
			EquipmentSlotType = UD1EquipManagerComponent::ConvertToEquipmentSlotType(ArmorType);
			break;
		case EEquipmentType::Weapon:
		case EEquipmentType::Utility:
			EquipmentSlotType = UD1EquipManagerComponent::ConvertToEquipmentSlotType(WeaponHandType, EquipManager->GetCurrentEquipState());
			break;
		}
	
		const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);
		const float NumItemsToConsumeReal = Quantity.GetValueAtLevel(AbilityLevel);
		const int32 NumItemsToConsume = FMath::TruncToInt(NumItemsToConsumeReal);
		
		EquipmentManager->RemoveEquipment_Unsafe(EquipmentSlotType, NumItemsToConsume);
	}
}
