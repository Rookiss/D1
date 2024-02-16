#include "D1EquipmentSlotsWidget.h"

#include "D1EquipmentSlotArmorWidget.h"
#include "Character/D1Player.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "UI/Item/Inventory/D1InventoryEntryWidget.h"
#include "UI/Item/Drag/D1ItemDragDrop.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentSlotsWidget)

UD1EquipmentSlotsWidget::UD1EquipmentSlotsWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1EquipmentSlotsWidget::Init()
{
	SlotWidgets = {
		Equipment_WeaponPrimary_LeftHand, Equipment_WeaponPrimary_RightHand, Equipment_WeaponPrimary_TwoHand,
		Equipment_WeaponSecondary_LeftHand, Equipment_WeaponSecondary_RightHand, Equipment_WeaponSecondary_TwoHand,
		Equipment_Armor_Head, Equipment_Armor_Chest, Equipment_Armor_Legs, Equipment_Armor_Hand, Equipment_Armor_Foot
	};

	for (int32 i = 0; i < SlotWidgets.Num(); i++)
	{
		UD1EquipmentSlotArmorWidget* SlotWidget = SlotWidgets[i];
		SlotWidget->Init(static_cast<EEquipmentSlotType>(i));
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

void UD1EquipmentSlotsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AD1Player* Player = Cast<AD1Player>(GetOwningPlayerPawn());
	check(Player);

	EquipmentManagerComponent = Player->EquipmentManagerComponent;
	check(EquipmentManagerComponent);
}

void UD1EquipmentSlotsWidget::OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance)
{
	UD1EquipmentSlotArmorWidget* SlotWidget = SlotWidgets[EquipmentSlotType];
	SlotWidget->OnEquipmentEntryChanged(ItemInstance);
}
