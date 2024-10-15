#include "D1GameplayAbility_Utility_Drink.h"

#include "Actors/D1WeaponBase.h"
#include "Character/LyraCharacter.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemInstance.h"
#include "Item/D1ItemTemplate.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Utility.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Utility_Drink)

UD1GameplayAbility_Utility_Drink::UD1GameplayAbility_Utility_Drink(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Utility_Drink::ApplyUtilityGameplayEffect()
{
	if (WeaponActor == nullptr)
		return;

	int32 ItemTemplateID = WeaponActor->GetTemplateID();
	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemTemplateID);

	const UD1ItemFragment_Equippable_Utility* UtilityFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Equippable_Utility>();
	if (UtilityFragment == nullptr)
		return;

	TSubclassOf<UGameplayEffect> UtilityEffectClass = UtilityFragment->UtilityEffectClass;
	if (UtilityEffectClass == nullptr)
		return;

	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	if (LyraCharacter == nullptr)
		return;

	UD1EquipManagerComponent* EquipManager = LyraCharacter->GetComponentByClass<UD1EquipManagerComponent>();
	UD1EquipmentManagerComponent* EquipmentManager = LyraCharacter->GetComponentByClass<UD1EquipmentManagerComponent>();
	if (EquipManager == nullptr || EquipmentManager == nullptr)
		return;

	if (UD1EquipManagerComponent::IsUtilityEquipState(EquipManager->GetCurrentEquipState()) == false)
		return;

	EEquipmentSlotType EquipmentSlotType = UD1EquipManagerComponent::ConvertToEquipmentSlotType(WeaponHandType, EquipManager->GetCurrentEquipState());
	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return;

	const UD1ItemInstance* ItemInstance = EquipmentManager->GetItemInstance(EquipmentSlotType);
	if (ItemInstance == nullptr)
		return;

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(UtilityEffectClass, (float)ItemInstance->GetItemRarity());
	ApplyGameplayEffectSpecToOwner(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), SpecHandle);
}
