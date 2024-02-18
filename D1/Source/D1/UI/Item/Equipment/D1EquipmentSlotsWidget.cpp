#include "D1EquipmentSlotsWidget.h"

#include "D1EquipmentSlotArmorWidget.h"
#include "D1EquipmentSlotWeaponWidget.h"
#include "Character/D1Player.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
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
		SlotWeaponWidgets[i]->Init(static_cast<EWeaponSlotType>(i));
	}

	for (int32 i = 0; i < SlotArmorWidgets.Num(); i++)
	{
		SlotArmorWidgets[i]->Init(static_cast<EArmorType>(i));
	}

	AD1Player* Player = Cast<AD1Player>(GetOwningPlayerPawn());
	check(Player);

	EquipmentManagerComponent = Player->EquipmentManagerComponent;
	check(EquipmentManagerComponent);
	
	const TArray<FD1EquipmentEntry>& Entries = EquipmentManagerComponent->GetAllEntries();
	for (int32 i = 0; i < Entries.Num(); i++)
	{
		const FD1EquipmentEntry& Entry = Entries[i];
		if (UD1ItemInstance* ItemInstance = Entry.GetItemInstance())
		{
			OnEquipmentEntryChanged(static_cast<EEquipmentSlotType>(i), ItemInstance);
		}
	}
	EquipmentManagerComponent->OnEquipmentEntryChanged.AddUObject(this, &ThisClass::OnEquipmentEntryChanged);
}

void UD1EquipmentSlotsWidget::OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance)
{
	if (EquipmentSlotType == EquipmentSlotCount)
		return;

	int32 WeaponSlotCount = static_cast<int32>(EWeaponSlotType::Count) * static_cast<int32>(EWeaponHandType::Count);
	if (EquipmentSlotType < WeaponSlotCount)
	{
		int32 WeaponSlotIndex = EquipmentSlotType / static_cast<int32>(EWeaponHandType::Count);
		int32 WeaponHandIndex = EquipmentSlotType % static_cast<int32>(EWeaponHandType::Count);
		UD1EquipmentSlotWeaponWidget* SlotWeaponWidget = SlotWeaponWidgets[WeaponSlotIndex];
		SlotWeaponWidget->OnEquipmentEntryChanged(static_cast<EWeaponHandType>(WeaponHandIndex), ItemInstance);
	}
	else
	{
		UD1EquipmentSlotArmorWidget* SlotArmorWidget = SlotArmorWidgets[EquipmentSlotType - WeaponSlotCount];
		SlotArmorWidget->OnEquipmentEntryChanged(ItemInstance);
	}
}
