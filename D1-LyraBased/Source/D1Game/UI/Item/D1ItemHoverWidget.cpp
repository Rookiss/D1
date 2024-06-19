#include "D1ItemHoverWidget.h"

#include "D1ItemEntryWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Data/D1ItemData.h"
#include "GameFramework/GameplayMessageSubsystem.h"
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
		FString AttributeString;
		
		const FGameplayTagStackContainer& StackContainer = ItemInstance->GetStatContainer();
		for (const FGameplayTagStack& Stack : StackContainer.GetStacks())
		{
			FString Left, Right;
			Stack.GetStackTag().ToString().Split(TEXT("."), &Left, &Right, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
			AttributeString.Append(FString::Printf(TEXT("%s %d\n"), *Right, Stack.GetStackCount()));
		}
		AttributeString.RemoveFromEnd(TEXT("\n"));
		
		if (AttributeString.IsEmpty() == false)
		{
			Text_AttributeModifiers->SetText(FText::FromString(AttributeString));
			Text_AttributeModifiers->SetVisibility(ESlateVisibility::Visible);
		}
		
		if (EquippableFragment->EquipmentType == EEquipmentType::Weapon)
		{
			const UD1ItemFragment_Equippable_Weapon* WeaponFragment = Cast<UD1ItemFragment_Equippable_Weapon>(EquippableFragment);
			Text_ItemType->SetText(FText::FromString(TEXT("Weapon")));
			
			FString WeaponTypeString;
			switch (WeaponFragment->WeaponType)
			{
			case EWeaponType::Sword:		    WeaponTypeString = TEXT("검");		break;
			case EWeaponType::Shield:		    WeaponTypeString = TEXT("방패");	break;
			case EWeaponType::Bow:				WeaponTypeString = TEXT("활");	    break;
			}
			Text_WeaponType->SetText(FText::FromString(WeaponTypeString));
			HorizontalBox_WeaponType->SetVisibility(ESlateVisibility::Visible);
	
			FString WeaponHandTypeString;
			switch (WeaponFragment->WeaponHandType)
			{
			case EWeaponHandType::LeftHand:		WeaponHandTypeString = TEXT("왼손");	break;
			case EWeaponHandType::RightHand:	WeaponHandTypeString = TEXT("오른손");	break;
			case EWeaponHandType::TwoHand:		WeaponHandTypeString = TEXT("양손");	break;
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
			case EArmorType::Helmet:	        ArmorTypeString = TEXT("헬맷");	break;
			case EArmorType::Chest:		        ArmorTypeString = TEXT("가슴");	break;
			case EArmorType::Legs:		        ArmorTypeString = TEXT("다리");	break;
			case EArmorType::Hands:		        ArmorTypeString = TEXT("장갑");	break;
			case EArmorType::Foot:		        ArmorTypeString = TEXT("신발");	break;
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

void UD1ItemHoverWidget::SetPosition(const FVector2D& AbsolutePosition)
{
	FVector2D HoverWidgetSize = Overlay_Root->GetCachedGeometry().GetLocalSize();
	if (HoverWidgetSize.IsZero())
		return;
	
	FVector2D Margin = FVector2D(5.f, 5.f);
	FVector2D MouseWidgetPos = CanvasPanel_Root->GetCachedGeometry().AbsoluteToLocal(AbsolutePosition);
	FVector2D CanvasWidgetSize = CanvasPanel_Root->GetCachedGeometry().GetLocalSize();

	FVector2D HoverWidgetStartPos = MouseWidgetPos + Margin;
	FVector2D HoverWidgetEndPos = HoverWidgetStartPos + HoverWidgetSize;

	if (HoverWidgetEndPos.X > CanvasWidgetSize.X)
	{
		HoverWidgetStartPos.X = MouseWidgetPos.X - (Margin.X + HoverWidgetSize.X);
	}

	if (HoverWidgetEndPos.Y > CanvasWidgetSize.Y)
	{
		HoverWidgetStartPos.Y = MouseWidgetPos.Y - (Margin.Y + HoverWidgetSize.Y);
	}
		
	if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Overlay_Root->Slot))
	{
		CanvasPanelSlot->SetPosition(HoverWidgetStartPos);
	}
}
