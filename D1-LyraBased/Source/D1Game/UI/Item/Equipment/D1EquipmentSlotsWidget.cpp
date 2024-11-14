#include "D1EquipmentSlotsWidget.h"

#include "CommonVisibilitySwitcher.h"
#include "D1EquipmentSlotSingleWidget.h"
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

	WeaponSlotWidgets  = { Widget_Weapon_Primary, Widget_Weapon_Secondary };
	ArmorSlotWidgets   = { Widget_Armor_Head, Widget_Armor_Chest, Widget_Armor_Legs, Widget_Armor_Hand, Widget_Armor_Foot };
	UtilitySlotWidgets = { Widget_Utility_Primary, Widget_Utility_Secondary, Widget_Utility_Tertiary, Widget_Utility_Quaternary };
}

void UD1EquipmentSlotsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageListenerHandle = MessageSubsystem.RegisterListener(MessageChannelTag, this, &ThisClass::ConstructUI);
}

void UD1EquipmentSlotsWidget::NativeDestruct()
{
	DestructUI();
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(MessageListenerHandle);
	
	Super::NativeDestruct();
}

void UD1EquipmentSlotsWidget::ConstructUI(FGameplayTag Channel, const FEquipmentInitializeMessage& Message)
{
	if (Message.EquipmentManager == nullptr || Message.EquipManager == nullptr)
		return;

	EquipmentManager = Message.EquipmentManager;
	EquipManager = Message.EquipManager;
	
	for (int32 i = 0; i < WeaponSlotWidgets.Num(); i++)
	{
		WeaponSlotWidgets[i]->Init((EWeaponSlotType)i, EquipmentManager);
	}
	
	for (int32 i = 0; i < ArmorSlotWidgets.Num(); i++)
	{
		ArmorSlotWidgets[i]->Init((EArmorType)i, EquipmentManager);
	}

	for (int32 i = 0; i < UtilitySlotWidgets.Num(); i++)
	{
		UtilitySlotWidgets[i]->Init((EUtilitySlotType)i, EquipmentManager);
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
	EntryChangedDelegateHandle = EquipmentManager->OnEquipmentEntryChanged.AddUObject(this, &ThisClass::OnEquipmentEntryChanged);

	EEquipState CurrentEquipState = EquipManager->GetCurrentEquipState();
	OnEquipStateChanged(CurrentEquipState, CurrentEquipState);
	EquipStateChangedDelegateHandle = EquipManager->OnEquipStateChanged.AddUObject(this, &ThisClass::OnEquipStateChanged);
}

void UD1EquipmentSlotsWidget::DestructUI()
{
	if (EquipmentManager)
	{
		EquipmentManager->OnEquipmentEntryChanged.Remove(EntryChangedDelegateHandle);
		EntryChangedDelegateHandle.Reset();
	}
	
	if (EquipManager)
	{
		EquipManager->OnEquipStateChanged.Remove(EquipStateChangedDelegateHandle);
		EquipStateChangedDelegateHandle.Reset();
	}

	for (UD1EquipmentSlotWeaponWidget* SlotWeaponWidget : WeaponSlotWidgets)
	{
		if (SlotWeaponWidget)
		{
			for (int32 i = 0; i < (int32)EWeaponHandType::Count; i++)
			{
				SlotWeaponWidget->OnEquipmentEntryChanged((EWeaponHandType)i, nullptr, 0);
			}
		}
	}

	for (UD1EquipmentSlotSingleWidget* SlotArmorWidget : ArmorSlotWidgets)
	{
		if (SlotArmorWidget)
		{
			SlotArmorWidget->OnEquipmentEntryChanged(nullptr, 0);
		}
	}

	for (UD1EquipmentSlotSingleWidget* SlotUtilityWidget : UtilitySlotWidgets)
	{
		if (SlotUtilityWidget)
		{
			SlotUtilityWidget->OnEquipmentEntryChanged(nullptr, 0);
		}
	}
}

void UD1EquipmentSlotsWidget::OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance, int32 ItemCount)
{
	if (EquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || EquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || EquipmentSlotType == EEquipmentSlotType::Count)
		return;

	if (UD1EquipmentManagerComponent::IsWeaponSlot(EquipmentSlotType))
	{
		const int32 WeaponSlotIndex = (int32)UD1EquipManagerComponent::ConvertToWeaponSlotType(EquipmentSlotType);
		if (WeaponSlotWidgets.IsValidIndex(WeaponSlotIndex))
		{
			if (UD1EquipmentSlotWeaponWidget* WeaponSlotWidget = WeaponSlotWidgets[WeaponSlotIndex])
			{
				EWeaponHandType WeaponHandType = UD1EquipManagerComponent::ConvertToWeaponHandType(EquipmentSlotType);
				WeaponSlotWidget->OnEquipmentEntryChanged(WeaponHandType, ItemInstance, ItemCount);
			}
		}
	}
	else if (UD1EquipmentManagerComponent::IsArmorSlot(EquipmentSlotType))
	{
		const int32 ArmorSlotIndex = (int32)UD1EquipManagerComponent::ConvertToArmorType(EquipmentSlotType);
		if (ArmorSlotWidgets.IsValidIndex(ArmorSlotIndex))
		{
			if (UD1EquipmentSlotSingleWidget* ArmorSlotWidget = ArmorSlotWidgets[ArmorSlotIndex])
			{
				ArmorSlotWidget->OnEquipmentEntryChanged(ItemInstance, ItemCount);
			}
		}
	}
	else if (UD1EquipmentManagerComponent::IsUtilitySlot(EquipmentSlotType))
	{
		const int32 UtilitySlotIndex = (int32)UD1EquipManagerComponent::ConvertToUtilitySlotType(EquipmentSlotType);
		if (UtilitySlotWidgets.IsValidIndex(UtilitySlotIndex))
		{
			if (UD1EquipmentSlotSingleWidget* UtilitySlotWidget = UtilitySlotWidgets[UtilitySlotIndex])
			{
				UtilitySlotWidget->OnEquipmentEntryChanged(ItemInstance, ItemCount);
			}
		}
	}
}

void UD1EquipmentSlotsWidget::OnEquipStateChanged(EEquipState PrevEquipState, EEquipState NewEquipState)
{
	switch (NewEquipState)
	{
	case EEquipState::Weapon_Primary:
		Switcher_Weapon_Primary->SetActiveWidgetIndex(1);
		Switcher_Weapon_Secondary->SetActiveWidgetIndex(0);
		break;
	case EEquipState::Weapon_Secondary:
		Switcher_Weapon_Primary->SetActiveWidgetIndex(0);
		Switcher_Weapon_Secondary->SetActiveWidgetIndex(1);
		break;
	default:
		Switcher_Weapon_Primary->SetActiveWidgetIndex(0);
		Switcher_Weapon_Secondary->SetActiveWidgetIndex(0);
	}
}
