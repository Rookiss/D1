#include "D1ItemHoversWidget.h"

#include "D1ItemHoverWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equippable.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Armor.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Weapon.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemHoversWidget)

UD1ItemHoversWidget::UD1ItemHoversWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ItemHoversWidget::RefreshUI(const UD1ItemInstance* HoveredItemInstance)
{
	UD1ItemInstance* CompareItemInstance = nullptr;
	
	if (HoveredItemInstance)
	{
		UD1EquipManagerComponent* EquipManager = GetOwningPlayerPawn()->GetComponentByClass<UD1EquipManagerComponent>();
		UD1EquipmentManagerComponent* EquipmentManager = GetOwningPlayer()->GetComponentByClass<UD1EquipmentManagerComponent>();
		
		if (EquipManager && EquipmentManager)
		{
			if (const UD1ItemFragment_Equippable* EquippableFragment = HoveredItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>())
			{
				UD1ItemInstance* EquippedItemInstance = nullptr;
				
				if (EquippableFragment->EquipmentType == EEquipmentType::Weapon)
				{
					const UD1ItemFragment_Equippable_Weapon* WeaponFragment = Cast<UD1ItemFragment_Equippable_Weapon>(EquippableFragment);
					EquippedItemInstance = EquipManager->GetEquippedWeaponItemInstance(WeaponFragment->WeaponHandType);
					if (EquippedItemInstance && EquippedItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>()->WeaponType == EWeaponType::Unarmed)
					{
						EquippedItemInstance = nullptr;
					}
				}
				else if (EquippableFragment->EquipmentType == EEquipmentType::Armor)
				{
					const UD1ItemFragment_Equippable_Armor* ArmorFragment = Cast<UD1ItemFragment_Equippable_Armor>(EquippableFragment);
					EquippedItemInstance = EquipmentManager->GetItemInstance(UD1EquipManagerComponent::ConvertToEquipmentSlotType(ArmorFragment->ArmorType));
				}

				if (HoveredItemInstance != EquippedItemInstance)
				{
					CompareItemInstance = EquippedItemInstance;
				}
			}
		}
	}

	HoverWidget_Left->RefreshUI(HoveredItemInstance);
	HoverWidget_Right->RefreshUI(CompareItemInstance);
}

void UD1ItemHoversWidget::SetPosition(const FVector2D& AbsolutePosition)
{
	HorizontalBox_Hovers->ForceLayoutPrepass();
	FVector2D HoversWidgetSize = HorizontalBox_Hovers->GetDesiredSize();

	FVector2D CanvasWidgetSize = CanvasPanel_Root->GetCachedGeometry().GetLocalSize();
	if (CanvasWidgetSize.IsZero())
		return;
	
	FVector2D Margin = FVector2D(5.f, 5.f);
	FVector2D MouseWidgetPos = CanvasPanel_Root->GetCachedGeometry().AbsoluteToLocal(AbsolutePosition);
	FVector2D HoverWidgetStartPos = MouseWidgetPos + Margin;
	FVector2D HoverWidgetEndPos = HoverWidgetStartPos + HoversWidgetSize;

	if (HoverWidgetEndPos.X > CanvasWidgetSize.X)
	{
		HoverWidgetStartPos.X = MouseWidgetPos.X - (Margin.X + HoversWidgetSize.X);
	}

	if (HoverWidgetEndPos.Y > CanvasWidgetSize.Y)
	{
		HoverWidgetStartPos.Y = MouseWidgetPos.Y - (Margin.Y + HoversWidgetSize.Y);
	}
		
	if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(HorizontalBox_Hovers->Slot))
	{
		CanvasPanelSlot->SetPosition(HoverWidgetStartPos);
	}
}
