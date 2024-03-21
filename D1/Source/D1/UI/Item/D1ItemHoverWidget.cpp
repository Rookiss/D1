#include "D1ItemHoverWidget.h"

#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Consumable.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Armor.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "Item/Fragments/D1ItemFragment_Stackable.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemHoverWidget)

UD1ItemHoverWidget::UD1ItemHoverWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ItemHoverWidget::RefreshUI(UD1ItemInstance* ItemInstance)
{
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FD1ItemDefinition& ItemDef = ItemData->FindItemDefByID(ItemInstance->GetItemID());
	const EItemRarity ItemRarity = ItemInstance->GetItemRarity();

	Text_DisplayName->SetText(ItemDef.DisplayName);
	Text_DisplayName->SetColorAndOpacity(FSlateColor(Item::ItemRarityColors[static_cast<int32>(ItemRarity)]));

	Text_AttributeModifiers->SetVisibility(ESlateVisibility::Collapsed);
	Text_Description->SetVisibility(ESlateVisibility::Collapsed);
	
	HorizontalBox_WeaponType->SetVisibility(ESlateVisibility::Collapsed);
	HorizontalBox_WeaponHandType->SetVisibility(ESlateVisibility::Collapsed);
	HorizontalBox_ArmorType->SetVisibility(ESlateVisibility::Collapsed);
	HorizontalBox_MaxStackCount->SetVisibility(ESlateVisibility::Collapsed);
	
	if (const UD1ItemFragment_Consumable* Consumable = ItemDef.FindFragmentByClass<UD1ItemFragment_Consumable>())
	{
		Text_ItemType->SetText(FText::FromString(TEXT("Comsumable")));
		
		Text_Description->SetText(Consumable->Description);
		Text_Description->SetVisibility(ESlateVisibility::Visible);
		
		const UD1ItemFragment_Stackable* Stackable = ItemDef.FindFragmentByClass<UD1ItemFragment_Stackable>();
		Text_MaxStackCount->SetText(FText::AsNumber(Stackable ? Stackable->MaxStackCount : 1));
		HorizontalBox_MaxStackCount->SetVisibility(ESlateVisibility::Visible);
	}
	else if (const UD1ItemFragment_Equippable* Equippable = ItemDef.FindFragmentByClass<UD1ItemFragment_Equippable>())
	{
		FString AttributeModifiers;
		
		const FD1GameplayTagStackContainer& StackContainer = ItemInstance->GetStatContainer();
		for (const FD1GameplayTagStack& Stack : StackContainer.GetStacks())
		{
			FString Left, Right;
			Stack.GetStatTag().ToString().Split(TEXT("."), &Left, &Right, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
			AttributeModifiers.Append(FString::Printf(TEXT("%s %d\n"), *Right, Stack.GetStatCount()));
		}
		AttributeModifiers.RemoveFromEnd(TEXT("\n"));
		
		if (AttributeModifiers.IsEmpty() == false)
		{
			Text_AttributeModifiers->SetText(FText::FromString(AttributeModifiers));
			Text_AttributeModifiers->SetVisibility(ESlateVisibility::Visible);
		}
		
		if (Equippable->EquipmentType == EEquipmentType::Weapon)
		{
			const UD1ItemFragment_Equippable_Weapon* Weapon = Cast<UD1ItemFragment_Equippable_Weapon>(Equippable);
			Text_ItemType->SetText(FText::FromString(TEXT("Weapon")));

			FString WeaponType;
			switch (Weapon->WeaponType)
			{
			case EWeaponType::Sword:		WeaponType = TEXT("Sword");			break;
			case EWeaponType::MagicStuff:	WeaponType = TEXT("Magic Stuff");	break;
			case EWeaponType::Shield:		WeaponType = TEXT("Shield");		break;
			}
			Text_WeaponType->SetText(FText::FromString(WeaponType));
			HorizontalBox_WeaponType->SetVisibility(ESlateVisibility::Visible);

			FString WeaponHandType;
			switch (Weapon->WeaponHandType)
			{
			case EWeaponHandType::LeftHand:		WeaponHandType = TEXT("Left Hand");		break;
			case EWeaponHandType::RightHand:	WeaponHandType = TEXT("Right Hand");	break;
			case EWeaponHandType::TwoHand:		WeaponHandType = TEXT("Two Hand");		break;
			}
			Text_WeaponHandType->SetText(FText::FromString(WeaponHandType));
			HorizontalBox_WeaponHandType->SetVisibility(ESlateVisibility::Visible);
		}
		else if (Equippable->EquipmentType == EEquipmentType::Armor)
		{
			const UD1ItemFragment_Equippable_Armor* Armor = Cast<UD1ItemFragment_Equippable_Armor>(Equippable);
			Text_ItemType->SetText(FText::FromString(TEXT("Armor")));

			FString ArmorType;
			switch (Armor->ArmorType)
			{
			case EArmorType::Helmet:	ArmorType = TEXT("Head");	break;
			case EArmorType::Chest:		ArmorType = TEXT("Chest");	break;
			case EArmorType::Legs:		ArmorType = TEXT("Legs");	break;
			case EArmorType::Hands:		ArmorType = TEXT("Hand");	break;
			case EArmorType::Foot:		ArmorType = TEXT("Foot");	break;
			}
			Text_ArmorType->SetText(FText::FromString(ArmorType));
			HorizontalBox_ArmorType->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		Text_ItemType->SetText(FText::FromString(TEXT("Material")));
	}
}
