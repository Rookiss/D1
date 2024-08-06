#include "D1ItemFragment_Equippable_Utility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemFragment_Equippable_Utility)

UD1ItemFragment_Equippable_Utility::UD1ItemFragment_Equippable_Utility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    EquipmentType = EEquipmentType::Utility;
}

void UD1ItemFragment_Equippable_Utility::OnInstanceCreated(UD1ItemInstance* ItemInstance) const
{
	Super::OnInstanceCreated(ItemInstance);
	
	AddStatTagStack(ItemInstance, RarityStatSets);
}
