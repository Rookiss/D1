#include "D1ItemHoverWidget.h"

#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Consumable.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Armor.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "Item/Fragments/D1ItemFragment_Stackable.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemHoverWidget)

UD1ItemHoverWidget::UD1ItemHoverWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ItemHoverWidget::RefreshUI(UD1ItemInstance* ItemInstance)
{
	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
	const EItemRarity ItemRarity = ItemInstance->GetItemRarity();
	
	Text_DisplayName->SetText(ItemTemplate.DisplayName);
	Text_DisplayName->SetColorAndOpacity(FSlateColor(Item::ItemRarityColors[(int32)ItemRarity]));
	
	Text_AttributeModifiers->SetVisibility(ESlateVisibility::Collapsed);
	Text_Description->SetVisibility(ESlateVisibility::Collapsed);
	
	HorizontalBox_WeaponType->SetVisibility(ESlateVisibility::Collapsed);
	HorizontalBox_WeaponHandType->SetVisibility(ESlateVisibility::Collapsed);
	HorizontalBox_ArmorType->SetVisibility(ESlateVisibility::Collapsed);
	HorizontalBox_MaxStackCount->SetVisibility(ESlateVisibility::Collapsed);
	
	if (const UD1ItemFragment_Equippable* EquippableFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Equippable>())
	{
		FString AttributeModifierString;
		
		const FGameplayTagStackContainer& StackContainer = ItemInstance->GetStatContainer();
		for (const FGameplayTagStack& Stack : StackContainer.GetStacks())
		{
			FString Left, Right;
			Stack.GetStackTag().ToString().Split(TEXT("."), &Left, &Right, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
			AttributeModifierString.Append(FString::Printf(TEXT("%s %d\n"), *Right, Stack.GetStackCount()));
		}
		AttributeModifierString.RemoveFromEnd(TEXT("\n"));
		
		if (AttributeModifierString.IsEmpty() == false)
		{
			Text_AttributeModifiers->SetText(FText::FromString(AttributeModifierString));
			Text_AttributeModifiers->SetVisibility(ESlateVisibility::Visible);
		}
		
		if (EquippableFragment->EquipmentType == EEquipmentType::Weapon)
		{
			const UD1ItemFragment_Equippable_Weapon* WeaponFragment = Cast<UD1ItemFragment_Equippable_Weapon>(EquippableFragment);
			Text_ItemType->SetText(FText::FromString(TEXT("Weapon")));
	
			FString WeaponTypeString;
			switch (WeaponFragment->WeaponType)
			{
			case EWeaponType::Sword:		    WeaponTypeString = TEXT("Sword");		    break;
			case EWeaponType::Shield:		    WeaponTypeString = TEXT("Shield");		    break;
			}
			Text_WeaponType->SetText(FText::FromString(WeaponTypeString));
			HorizontalBox_WeaponType->SetVisibility(ESlateVisibility::Visible);
	
			FString WeaponHandTypeString;
			switch (WeaponFragment->WeaponHandType)
			{
			case EWeaponHandType::LeftHand:		WeaponHandTypeString = TEXT("Left Hand");	break;
			case EWeaponHandType::RightHand:	WeaponHandTypeString = TEXT("Right Hand");	break;
			case EWeaponHandType::TwoHand:		WeaponHandTypeString = TEXT("Two Hand");	break;
			}
			Text_WeaponHandType->SetText(FText::FromString(WeaponHandTypeString));
			HorizontalBox_WeaponHandType->SetVisibility(ESlateVisibility::Visible);
		}
		else if (EquippableFragment->EquipmentType == EEquipmentType::Armor)
		{
			const UD1ItemFragment_Equippable_Armor* ArmorFragment = Cast<UD1ItemFragment_Equippable_Armor>(EquippableFragment);
			Text_ItemType->SetText(FText::FromString(TEXT("Armor")));
	
			FString ArmorTypeString;
			switch (ArmorFragment->ArmorType)
			{
			case EArmorType::Helmet:	        ArmorTypeString = TEXT("Head");				break;
			case EArmorType::Chest:		        ArmorTypeString = TEXT("Chest");            break;
			case EArmorType::Legs:		        ArmorTypeString = TEXT("Legs");				break;
			case EArmorType::Hands:		        ArmorTypeString = TEXT("Hand");	            break;
			case EArmorType::Foot:		        ArmorTypeString = TEXT("Foot");				break;
			}       
			Text_ArmorType->SetText(FText::FromString(ArmorTypeString));
			HorizontalBox_ArmorType->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (const UD1ItemFragment_Consumable* ConsumableFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Consumable>())
	{
		Text_ItemType->SetText(FText::FromString(TEXT("Comsumable")));
		
		Text_Description->SetText(ConsumableFragment->Description);
		Text_Description->SetVisibility(ESlateVisibility::Visible);
	}

	if (const UD1ItemFragment_Stackable* StackableFragment = ItemTemplate.FindFragmentByClass<UD1ItemFragment_Stackable>())
	{
		Text_MaxStackCount->SetText(FText::AsNumber(StackableFragment ? StackableFragment->MaxStackCount : 1));
		HorizontalBox_MaxStackCount->SetVisibility(ESlateVisibility::Visible);
	}
}
