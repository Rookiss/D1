#include "D1EquipmentSlotsWidget.h"

#include "D1EquipmentSlotArmorWidget.h"
#include "D1EquipmentSlotWeaponWidget.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Player/D1PlayerController.h"
#include "UI/Item/Drag/D1ItemDragDrop.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentSlotsWidget)

UD1EquipmentSlotsWidget::UD1EquipmentSlotsWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1EquipmentSlotsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SlotWeaponWidgets = { Equipment_Weapon_Primary, Equipment_Weapon_Secondary };
	SlotArmorWidgets  = { Equipment_Armor_Head, Equipment_Armor_Chest, Equipment_Armor_Legs, Equipment_Armor_Hand, Equipment_Armor_Foot };

	for (int32 i = 0; i < SlotWeaponWidgets.Num(); i++)
	{
		SlotWeaponWidgets[i]->Init((EWeaponSlotType)i);
	}

	for (int32 i = 0; i < SlotArmorWidgets.Num(); i++)
	{
		SlotArmorWidgets[i]->Init((EArmorType)i);
	}
	
	AD1PlayerController* PC = Cast<AD1PlayerController>(GetOwningPlayer());
	check(PC);

	EquipmentManagerComponent = PC->EquipmentManagerComponent;
	check(EquipmentManagerComponent);
	
	const TArray<FD1EquipmentEntry>& Entries = EquipmentManagerComponent->GetAllEntries();
	for (int32 i = 0; i < Entries.Num(); i++)
	{
		const FD1EquipmentEntry& Entry = Entries[i];
		if (UD1ItemInstance* ItemInstance = Entry.GetItemInstance())
		{
			OnEquipmentEntryChanged((EEquipmentSlotType)i, ItemInstance);
		}
	}
	EquipmentManagerComponent->OnEquipmentEntryChanged.AddUObject(this, &ThisClass::OnEquipmentEntryChanged);
}

void UD1EquipmentSlotsWidget::OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance)
{
	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return;

	// TODO : Check Unarmed Slot
	const int32 EquipmentSlotIndex = (int32)EquipmentSlotType;
	const int32 WeaponSlotCount = (int32)EWeaponSlotType::Count * (int32)EWeaponHandType::Count;
	if (EquipmentSlotIndex < WeaponSlotCount)
	{
		const int32 WeaponSlotIndex = EquipmentSlotIndex / (int32)EWeaponHandType::Count;
		const int32 WeaponHandIndex = EquipmentSlotIndex % (int32)EWeaponHandType::Count;
		UD1EquipmentSlotWeaponWidget* SlotWeaponWidget = SlotWeaponWidgets[WeaponSlotIndex];
		SlotWeaponWidget->OnEquipmentEntryChanged((EWeaponHandType)WeaponHandIndex, ItemInstance);
	}
	else
	{
		UD1EquipmentSlotArmorWidget* SlotArmorWidget = SlotArmorWidgets[EquipmentSlotIndex - WeaponSlotCount];
		SlotArmorWidget->OnEquipmentEntryChanged(ItemInstance);
	}
}
