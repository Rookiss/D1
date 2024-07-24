#include "D1CheatMenuWidget.h"

#include "D1CheatListWidget.h"
#include "Components/ScrollBox.h"
#include "Item/D1ItemTemplate.h"
#include "Item/Fragments/D1ItemFragment_Equippable.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1CheatMenuWidget)

UD1CheatMenuWidget::UD1CheatMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1CheatMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const UD1ItemData& ItemData = ULyraAssetManager::Get().GetItemData();

	TArray<TSubclassOf<UD1ItemTemplate>> ItemTemplateClasses;
	ItemData.GetAllItemTemplateClasses(ItemTemplateClasses);

	for (TSubclassOf<UD1ItemTemplate>& ItemTemplateClass : ItemTemplateClasses)
	{
		const UD1ItemTemplate* ItemTemplate = ItemTemplateClass.GetDefaultObject();
		if (const UD1ItemFragment_Equippable* EquippableFragment = ItemTemplate->FindFragmentByClass<UD1ItemFragment_Equippable>())
		{
			if (EquippableFragment->EquipmentType == EEquipmentType::Weapon)
			{
				const UD1ItemFragment_Equippable_Weapon* WeaponFragment = ItemTemplate->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>();
				
				// CheatList_PrimaryWeapon->ScrollBox_Entries->AddChild();
			}
			else if (EquippableFragment->EquipmentType == EEquipmentType::Armor)
			{
				
			}
		}
	}
}
