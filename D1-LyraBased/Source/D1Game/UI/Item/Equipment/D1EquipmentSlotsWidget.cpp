#include "D1EquipmentSlotsWidget.h"

#include "D1EquipmentSlotArmorWidget.h"
#include "D1EquipmentSlotWeaponWidget.h"
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
	
	for (int32 i = 0; i < SlotWeaponWidgets.Num(); i++)
	{
		SlotWeaponWidgets[i]->Init((EWeaponSlotType)i);
	}
	
	for (int32 i = 0; i < SlotArmorWidgets.Num(); i++)
	{
		SlotArmorWidgets[i]->Init((EArmorType)i);
	}
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

	const TArray<FD1EquipmentEntry>& Entries = EquipmentManager->GetAllEntries();
	for (int32 i = 0; i < Entries.Num(); i++)
	{
		const FD1EquipmentEntry& Entry = Entries[i];
		if (UD1ItemInstance* ItemInstance = Entry.GetItemInstance())
		{
			OnEquipmentEntryChanged((EEquipmentSlotType)i, ItemInstance);
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
			SlotArmorWidget->OnEquipmentEntryChanged(nullptr);
		}
	}
}

void UD1EquipmentSlotsWidget::OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance)
{
	if (EquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || EquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || EquipmentSlotType == EEquipmentSlotType::Count)
		return;
	
	const int32 EquipmentSlotIndex = (int32)EquipmentSlotType;
	const int32 WeaponSlotCount = (int32)EWeaponSlotType::Count * (int32)EWeaponHandType::Count;
	const int32 ArmorStartIndex = WeaponSlotCount + (int32)EEquipmentSlotType::Primary_LeftHand;
	
	if (EquipmentSlotIndex < ArmorStartIndex)
	{
		const int32 ZeroBasedIndex = EquipmentSlotIndex - (int32)EEquipmentSlotType::Primary_LeftHand;
		const int32 WeaponSlotIndex = ZeroBasedIndex / (int32)EWeaponHandType::Count;
		const int32 WeaponHandIndex = ZeroBasedIndex % (int32)EWeaponHandType::Count;

		if (SlotWeaponWidgets.IsValidIndex(WeaponSlotIndex))
		{
			UD1EquipmentSlotWeaponWidget* SlotWeaponWidget = SlotWeaponWidgets[WeaponSlotIndex];
			SlotWeaponWidget->OnEquipmentEntryChanged((EWeaponHandType)WeaponHandIndex, ItemInstance);
		}
	}
	else
	{
		UD1EquipmentSlotArmorWidget* SlotArmorWidget = SlotArmorWidgets[EquipmentSlotIndex - ArmorStartIndex];
		SlotArmorWidget->OnEquipmentEntryChanged(ItemInstance);
	}
}
