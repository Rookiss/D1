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
	UD1ItemInstance* SelectedItemInstance = nullptr;
	
	if (HoveredItemInstance)
	{
		UD1EquipManagerComponent* EquipManager = GetOwningPlayerPawn()->GetComponentByClass<UD1EquipManagerComponent>();
		UD1EquipmentManagerComponent* EquipmentManager = GetOwningPlayerPawn()->GetComponentByClass<UD1EquipmentManagerComponent>();
		
		if (EquipManager && EquipmentManager)
		{
			if (const UD1ItemFragment_Equippable* HoveredEquippableFragment = HoveredItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable>())
			{
				if (HoveredEquippableFragment->EquipmentType == EEquipmentType::Weapon)
				{
					const UD1ItemFragment_Equippable_Weapon* HoveredWeaponFragment = Cast<UD1ItemFragment_Equippable_Weapon>(HoveredEquippableFragment);

					UD1ItemInstance* BestPickItemInstance = nullptr;
					UD1ItemInstance* SecondPickItemInstance = nullptr;
					
					TArray<UD1ItemInstance*> WeaponItemInstances;
					EquipManager->GetAllEquippedWeaponItemInstances(WeaponItemInstances);

					for (UD1ItemInstance* WeaponItemInstance : WeaponItemInstances)
					{
						const UD1ItemFragment_Equippable_Weapon* EquippedWeaponFragment = WeaponItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>();
						if (HoveredWeaponFragment->WeaponType == EquippedWeaponFragment->WeaponType)
						{
							if (HoveredWeaponFragment->WeaponHandType == EquippedWeaponFragment->WeaponHandType)
							{
								BestPickItemInstance = WeaponItemInstance;
								break;
							}

							if (SecondPickItemInstance == nullptr)
							{
								SecondPickItemInstance = WeaponItemInstance;
							}
						}
					}

					if (BestPickItemInstance == nullptr)
					{
						EquipmentManager->GetAllWeaponItemInstances(WeaponItemInstances);

						for (UD1ItemInstance* WeaponItemInstance : WeaponItemInstances)
						{
							const UD1ItemFragment_Equippable_Weapon* EquippedWeaponFragment = WeaponItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Weapon>();
							if (HoveredWeaponFragment->WeaponType == EquippedWeaponFragment->WeaponType)
							{
								if (HoveredWeaponFragment->WeaponHandType == EquippedWeaponFragment->WeaponHandType)
								{
									BestPickItemInstance = WeaponItemInstance;
									break;
								}
							
								if (SecondPickItemInstance == nullptr)
								{
									SecondPickItemInstance = WeaponItemInstance;
								}
							}
						}
					}

					SelectedItemInstance = (BestPickItemInstance ? BestPickItemInstance : SecondPickItemInstance);
				}
				else if (HoveredEquippableFragment->EquipmentType == EEquipmentType::Armor)
				{
					const UD1ItemFragment_Equippable_Armor* HoveredArmorFragment = Cast<UD1ItemFragment_Equippable_Armor>(HoveredEquippableFragment);
					SelectedItemInstance = EquipmentManager->GetItemInstance(UD1EquipManagerComponent::ConvertToEquipmentSlotType(HoveredArmorFragment->ArmorType));
				}

				if (HoveredItemInstance == SelectedItemInstance)
				{
					SelectedItemInstance = nullptr;
				}
			}
		}
	}

	HoverWidget_Left->RefreshUI(HoveredItemInstance);
	HoverWidget_Right->RefreshUI(SelectedItemInstance);
}

void UD1ItemHoversWidget::SetPosition(const FVector2D& AbsolutePosition)
{
	HorizontalBox_Hovers->ForceLayoutPrepass();
	FVector2D HoversWidgetSize = HorizontalBox_Hovers->GetDesiredSize();

	FVector2D CanvasWidgetSize = CanvasPanel_Root->GetCachedGeometry().GetLocalSize();
	if (CanvasWidgetSize.IsZero())
		return;
	
	FVector2D Margin = FVector2D(10.f, 15.f);
	FVector2D MouseWidgetPos = CanvasPanel_Root->GetCachedGeometry().AbsoluteToLocal(AbsolutePosition);
	FVector2D HoverWidgetStartPos = MouseWidgetPos + Margin;
	FVector2D HoverWidgetEndPos = HoverWidgetStartPos + HoversWidgetSize;

	FVector2D OutSize = (HoverWidgetEndPos - CanvasWidgetSize);
	if (OutSize.X > 0.f)
	{
		HoverWidgetStartPos.X -= OutSize.X;
	}
	if (OutSize.Y > 0.f)
	{
		HoverWidgetStartPos.Y -= OutSize.Y;
	}
		
	if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(HorizontalBox_Hovers->Slot))
	{
		CanvasPanelSlot->SetPosition(HoverWidgetStartPos);
	}
}
