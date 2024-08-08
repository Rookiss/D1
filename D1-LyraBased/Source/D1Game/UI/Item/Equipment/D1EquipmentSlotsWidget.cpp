#include "D1EquipmentSlotsWidget.h"

#include "D1EquipmentSlotArmorWidget.h"
#include "D1EquipmentSlotWeaponWidget.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentSlotsWidget)

UD1EquipmentSlotsWidget::UD1EquipmentSlotsWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1EquipmentSlotsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SlotWeaponWidgets = { Equipment_Weapon_Primary, Equipment_Weapon_Secondary };
	SlotArmorWidgets  = { Equipment_Armor_Head, Equipment_Armor_Chest, Equipment_Armor_Legs, Equipment_Armor_Hand, Equipment_Armor_Foot };
}

void UD1EquipmentSlotsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	ListenerHandle = MessageSubsystem.RegisterListener(MessageChannelTag, this, &ThisClass::ConstructUI);
}

void UD1EquipmentSlotsWidget::NativeDestruct()
{
	DestructUI();
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(ListenerHandle);
	
	Super::NativeDestruct();
}

void UD1EquipmentSlotsWidget::ConstructUI(FGameplayTag Channel, const FEquipmentInitializeMessage& Message)
{
	if (Message.EquipmentManager == nullptr)
		return;

	EquipmentManager = Message.EquipmentManager;
	
	for (int32 i = 0; i < SlotWeaponWidgets.Num(); i++)
	{
		SlotWeaponWidgets[i]->Init((EWeaponSlotType)i, EquipmentManager);
	}
	
	for (int32 i = 0; i < SlotArmorWidgets.Num(); i++)
	{
		SlotArmorWidgets[i]->Init((EArmorType)i, EquipmentManager);
	}

	const TArray<FD1EquipmentEntry>& Entries = EquipmentManager->GetAllEntries();
	for (int32 i = 0; i < Entries.Num(); i++)
	{
		const FD1EquipmentEntry& Entry = Entries[i];
		if (UD1ItemInstance* ItemInstance = Entry.GetItemInstance())
		{
			OnEquipmentEntryChanged((EEquipmentSlotType)i, ItemInstance, Entry.GetItemCount());
		}
	}
	DelegateHandle = EquipmentManager->OnEquipmentEntryChanged.AddUObject(this, &ThisClass::OnEquipmentEntryChanged);
}

void UD1EquipmentSlotsWidget::DestructUI()
{
	EquipmentManager->OnEquipmentEntryChanged.Remove(DelegateHandle);
	DelegateHandle.Reset();

	for (UD1EquipmentSlotWeaponWidget* SlotWeaponWidget : SlotWeaponWidgets)
	{
		if (SlotWeaponWidget)
		{
			for (int32 i = 0; i < (int32)EWeaponHandType::Count; i++)
			{
				SlotWeaponWidget->OnEquipmentEntryChanged((EWeaponHandType)i, nullptr);
			}
		}
	}

	for (UD1EquipmentSlotArmorWidget* SlotArmorWidget : SlotArmorWidgets)
	{
		if (SlotArmorWidget)
		{
			SlotArmorWidget->OnEquipmentEntryChanged(nullptr, 0);
		}
	}
}

void UD1EquipmentSlotsWidget::OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance, int32 ItemCount)
{
	if (EquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || EquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || EquipmentSlotType == EEquipmentSlotType::Count)
		return;

	if (UD1EquipmentManagerComponent::IsWeaponSlot(EquipmentSlotType))
	{
		int32 WeaponSlotIndex = (int32)UD1EquipManagerComponent::ConvertToWeaponSlotType(EquipmentSlotType);
		if (SlotWeaponWidgets.IsValidIndex(WeaponSlotIndex))
		{
			UD1EquipmentSlotWeaponWidget* SlotWeaponWidget = SlotWeaponWidgets[WeaponSlotIndex];
			EWeaponHandType WeaponHandType = UD1EquipManagerComponent::ConvertToWeaponHandType(EquipmentSlotType);
			SlotWeaponWidget->OnEquipmentEntryChanged(WeaponHandType, ItemInstance);
		}
	}
	else if (UD1EquipmentManagerComponent::IsArmorSlot(EquipmentSlotType))
	{
		int32 ArmorSlotIndex = (int32)UD1EquipManagerComponent::ConvertToArmorType(EquipmentSlotType);
		if (SlotArmorWidgets.IsValidIndex(ArmorSlotIndex))
		{
			UD1EquipmentSlotArmorWidget* SlotArmorWidget = SlotArmorWidgets[ArmorSlotIndex];
			SlotArmorWidget->OnEquipmentEntryChanged(ItemInstance, 1);
		}
	}
}
