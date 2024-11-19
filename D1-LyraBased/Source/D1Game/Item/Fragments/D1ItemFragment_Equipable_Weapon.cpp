#include "D1ItemFragment_Equipable_Weapon.h"

#include "Item/D1ItemInstance.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemFragment_Equipable_Weapon)

UD1ItemFragment_Equipable_Weapon::UD1ItemFragment_Equipable_Weapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    EquipmentType = EEquipmentType::Weapon;
}

#if WITH_EDITORONLY_DATA
void UD1ItemFragment_Equipable_Weapon::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	for (int i = 0; i < RarityStatRangeSets.Num(); i++)
	{
		TArray<FRarityStatRange>& RarityStatRanges = RarityStatRangeSets[i].RarityStatRanges;
		RarityStatRanges.SetNum((int32)EItemRarity::Count);
		for (int32 j = 0; j < RarityStatRanges.Num(); j++)
		{
			RarityStatRanges[j].Rarity = (EItemRarity)j;
		}
	}
}
#endif // WITH_EDITORONLY_DATA

void UD1ItemFragment_Equipable_Weapon::OnInstanceCreated(UD1ItemInstance* ItemInstance) const
{
	Super::OnInstanceCreated(ItemInstance);
	
	AddStatTagStack(ItemInstance, RarityStatRangeSets);
}
