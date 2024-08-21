#include "D1GameplayAbility_Utility_Drink.h"

#include "Actors/D1WeaponBase.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemTemplate.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Utility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Utility_Drink)

UD1GameplayAbility_Utility_Drink::UD1GameplayAbility_Utility_Drink(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

TSubclassOf<UGameplayEffect> UD1GameplayAbility_Utility_Drink::GetUtilityEffectClass() const
{
	if (WeaponActor == nullptr)
		return nullptr;

	TSubclassOf<UGameplayEffect> UtilityEffectClass = nullptr;
	
	int32 ItemTemplateID = WeaponActor->GetTemplateID();
	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemTemplateID);

	if (const UD1ItemFragment_Equippable_Utility* UtilityFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Equippable_Utility>())
	{
		UtilityEffectClass = UtilityFragment->UtilityEffectClass;
	}
	return UtilityEffectClass;
}
