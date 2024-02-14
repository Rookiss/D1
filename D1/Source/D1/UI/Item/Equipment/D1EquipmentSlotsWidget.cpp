#include "D1EquipmentSlotsWidget.h"

#include "D1EquipmentSlotWidget.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "UI/Item/Inventory/D1InventoryEntryWidget.h"
#include "UI/Item/Drag/D1ItemDragDrop.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentSlotsWidget)

UD1EquipmentSlotsWidget::UD1EquipmentSlotsWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1EquipmentSlotsWidget::Init(UD1EquipmentManagerComponent* InEquipmentManagerComponent)
{
	check(InEquipmentManagerComponent);
	EquipmentManagerComponent = InEquipmentManagerComponent;
		
	SlotWidgets = {
		Equipment_WeaponPrimary_LeftHand, Equipment_WeaponPrimary_RightHand, Equipment_WeaponPrimary_TwoHand,
		Equipment_WeaponSecondary_LeftHand, Equipment_WeaponSecondary_RightHand, Equipment_WeaponSecondary_TwoHand,
		Equipment_Armor_Head, Equipment_Armor_Chest, Equipment_Armor_Legs, Equipment_Armor_Hand, Equipment_Armor_Foot
	};

	for (int32 i = 0; i < SlotWidgets.Num(); i++)
	{
		UD1EquipmentSlotWidget* SlotWidget = SlotWidgets[i];
		SlotWidget->Init(EquipmentManagerComponent, static_cast<EEquipmentSlotType>(i));
	}

	const TArray<FD1EquipmentEntry>& Entries = EquipmentManagerComponent->GetAllEntries();
	for (int32 i = 0; i < Entries.Num(); i++)
	{
		const FD1EquipmentEntry& Entry = Entries[i];
		if (Entry.GetItemInstance())
		{
			OnEquipmentEntryChanged(static_cast<EEquipmentSlotType>(i), Entry.GetItemInstance());
		}
	}
	EquipmentManagerComponent->OnEquipmentEntryChanged.AddUObject(this, &ThisClass::OnEquipmentEntryChanged);
}

void UD1EquipmentSlotsWidget::OnEquipmentEntryChanged_Implementation(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance)
{
	UD1EquipmentSlotWidget* SlotWidget = SlotWidgets[EquipmentSlotType];
	SlotWidget->OnEquipmentEntryChanged(ItemInstance);
}
