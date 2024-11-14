#include "D1WeaponSlotWidget.h"

#include "CommonVisibilitySwitcher.h"
#include "Character/LyraCharacter.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemTemplate.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1WeaponSlotWidget)

UD1WeaponSlotWidget::UD1WeaponSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1WeaponSlotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	FText SlotNumber = FText::AsNumber((int32)WidgetWeaponSlotType + 1);
	Text_OneSlot_Unselected_Number->SetText(SlotNumber);
	Text_OneSlot_Selected_Number->SetText(SlotNumber);
	Text_TwoSlot_Unselected_Number->SetText(SlotNumber);
	Text_TwoSlot_Selected_Number->SetText(SlotNumber);
}

void UD1WeaponSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetOwningPlayerPawn());
	if (LyraCharacter == nullptr)
		return;
	
	EquipManager = LyraCharacter->GetComponentByClass<UD1EquipManagerComponent>();
	EquipmentManager = LyraCharacter->GetComponentByClass<UD1EquipmentManagerComponent>();
	if (EquipManager == nullptr || EquipmentManager == nullptr)
		return;

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

void UD1WeaponSlotWidget::NativeDestruct()
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
	
	Super::NativeDestruct();
}

void UD1WeaponSlotWidget::OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, UD1ItemInstance* ItemInstance, int32 ItemCount)
{
	EWeaponSlotType EntryWeaponSlotType = UD1EquipManagerComponent::ConvertToWeaponSlotType(EquipmentSlotType);
	if (EntryWeaponSlotType != WidgetWeaponSlotType)
		return;
	
	EWeaponHandType EntryWeaponHandType = UD1EquipManagerComponent::ConvertToWeaponHandType(EquipmentSlotType);
	if (EntryWeaponHandType == EWeaponHandType::LeftHand)
	{
		if (ItemInstance)
		{
			const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());

			Image_TwoSlot_Unselected_1->SetBrushFromTexture(ItemTemplate.IconTexture, true);
			Image_TwoSlot_Unselected_1->SetVisibility(ESlateVisibility::HitTestInvisible);

			Image_TwoSlot_Selected_1->SetBrushFromTexture(ItemTemplate.IconTexture, true);
			Image_TwoSlot_Selected_1->SetVisibility(ESlateVisibility::HitTestInvisible);

			Switcher_Slots->SetActiveWidgetIndex(bIsSelected ? 3 : 2);
		}
		else
		{
			Image_TwoSlot_Unselected_1->SetVisibility(ESlateVisibility::Hidden);
			Image_TwoSlot_Selected_1->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else if (EntryWeaponHandType == EWeaponHandType::RightHand)
	{
		if (ItemInstance)
		{
			const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());

			Image_TwoSlot_Unselected_2->SetBrushFromTexture(ItemTemplate.IconTexture, true);
			Image_TwoSlot_Unselected_2->SetVisibility(ESlateVisibility::HitTestInvisible);

			Image_TwoSlot_Selected_2->SetBrushFromTexture(ItemTemplate.IconTexture, true);
			Image_TwoSlot_Selected_2->SetVisibility(ESlateVisibility::HitTestInvisible);
			
			Switcher_Slots->SetActiveWidgetIndex(bIsSelected ? 3 : 2);
		}
		else
		{
			Image_TwoSlot_Unselected_2->SetVisibility(ESlateVisibility::Hidden);
			Image_TwoSlot_Selected_2->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else if (EntryWeaponHandType == EWeaponHandType::TwoHand)
	{
		if (ItemInstance)
		{
			const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());

			Image_OneSlot_Unselected->SetBrushFromTexture(ItemTemplate.IconTexture, true);
			Image_OneSlot_Unselected->SetVisibility(ESlateVisibility::HitTestInvisible);

			Image_OneSlot_Selected->SetBrushFromTexture(ItemTemplate.IconTexture, true);
			Image_OneSlot_Selected->SetVisibility(ESlateVisibility::HitTestInvisible);

			Switcher_Slots->SetActiveWidgetIndex(bIsSelected ? 1 : 0);
		}
		else
		{
			Image_OneSlot_Unselected->SetVisibility(ESlateVisibility::Hidden);
			Image_OneSlot_Selected->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UD1WeaponSlotWidget::OnEquipStateChanged(EEquipState PrevEquipState, EEquipState NewEquipState)
{
	EEquipState SlotEquipState = UD1EquipManagerComponent::ConvertToEquipState(WidgetWeaponSlotType);
	bIsSelected = (NewEquipState == SlotEquipState);
	
	if (bIsSelected)
	{
		Switcher_Slots->IncrementActiveWidgetIndex(false);
	}
	else if (PrevEquipState == SlotEquipState)
	{
		Switcher_Slots->DecrementActiveWidgetIndex(false);
	}
}
