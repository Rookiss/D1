#include "D1ItemFragment_Equippable_Weapon.h"

#include "Item/D1ItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemFragment_Equippable_Weapon)

UD1ItemFragment_Equippable_Weapon::UD1ItemFragment_Equippable_Weapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    EquipmentType = EEquipmentType::Weapon;
}

void UD1ItemFragment_Equippable_Weapon::OnInstanceCreated(UD1ItemInstance* ItemInstance) const
{
	Super::OnInstanceCreated(ItemInstance);
	
	AddStatTagStack(ItemInstance, RarityStatRangeSets);
}
