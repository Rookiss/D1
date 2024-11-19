#include "D1ItemFragment_Equipable_Armor.h"

#include "Item/D1ItemInstance.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemFragment_Equipable_Armor)

UD1ItemFragment_Equipable_Armor::UD1ItemFragment_Equipable_Armor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    EquipmentType = EEquipmentType::Armor;
}

#if WITH_EDITORONLY_DATA
void UD1ItemFragment_Equipable_Armor::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	if (ArmorType != EArmorType::Chest)
	{
		bIsFullBody = false;
	}
	
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

void UD1ItemFragment_Equipable_Armor::OnInstanceCreated(UD1ItemInstance* ItemInstance) const
{
	Super::OnInstanceCreated(ItemInstance);
	
	AddStatTagStack(ItemInstance, RarityStatRangeSets);
}
