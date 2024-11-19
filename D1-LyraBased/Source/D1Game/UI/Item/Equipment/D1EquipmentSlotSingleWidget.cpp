#include "D1EquipmentSlotSingleWidget.h"

#include "D1EquipmentSlotsWidget.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equipable_Armor.h"
#include "Item/Fragments/D1ItemFragment_Equipable_Utility.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Item/Managers/D1InventoryManagerComponent.h"
#include "Item/Managers/D1ItemManagerComponent.h"
#include "System/LyraAssetManager.h"
#include "UI/Item/D1ItemDragDrop.h"
#include "UI/Item/Equipment/D1EquipmentEntryWidget.h"
#include "UI/Item/Inventory/D1InventoryEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentSlotSingleWidget)

UD1EquipmentSlotSingleWidget::UD1EquipmentSlotSingleWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1EquipmentSlotSingleWidget::Init(EArmorType InArmorType, UD1EquipmentManagerComponent* InEquipmentManager)
{
	check(InArmorType != EArmorType::Count && InEquipmentManager != nullptr);

	EquipmentSingleSlotType = EEquipmentSingleSlotType::Armor;
	ArmorType = InArmorType;
	EquipmentManager = InEquipmentManager;
}

void UD1EquipmentSlotSingleWidget::Init(EUtilitySlotType InUtilitySlotType, UD1EquipmentManagerComponent* InEquipmentManager)
{
	check(InUtilitySlotType != EUtilitySlotType::Count && InEquipmentManager != nullptr);

	EquipmentSingleSlotType = EEquipmentSingleSlotType::Utility;
	UtilitySlotType = InUtilitySlotType;
	EquipmentManager = InEquipmentManager;
}

void UD1EquipmentSlotSingleWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Image_BaseIcon)
	{
		Image_BaseIcon->SetBrushFromTexture(BaseIconTexture, true);
	}
}

bool UD1EquipmentSlotSingleWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
	
	if (bAlreadyHovered)
		return true;

	bAlreadyHovered = true;
	
	UD1ItemDragDrop* ItemDragDrop = Cast<UD1ItemDragDrop>(InOperation);
	if (ItemDragDrop == nullptr)
		return false;

	UD1ItemInstance* FromItemInstance = ItemDragDrop->FromItemInstance;
	if (FromItemInstance == nullptr)
		return false;
	
	bool bIsValid = false;
	EEquipmentSlotType ToEquipmentSlotType = GetEquipmentSlotType();
	if (ToEquipmentSlotType == EEquipmentSlotType::Count)
		return false;
	
	if (UD1InventoryManagerComponent* FromInventoryManager = ItemDragDrop->FromInventoryManager)
	{
		if (EquipmentManager->GetItemInstance(ToEquipmentSlotType))
		{
			FIntPoint OutToItemSlotPos;
			bIsValid = EquipmentManager->CanSwapEquipment(FromInventoryManager, ItemDragDrop->FromItemSlotPos, ToEquipmentSlotType, OutToItemSlotPos);
		}
		else
		{
			bIsValid = EquipmentManager->CanMoveOrMergeEquipment(FromInventoryManager, ItemDragDrop->FromItemSlotPos, ToEquipmentSlotType) > 0;
		}
	}
	else if (UD1EquipmentManagerComponent* FromEquipmentManager = ItemDragDrop->FromEquipmentManager)
	{
		if (EquipmentManager->GetItemInstance(ToEquipmentSlotType))
		{
			bIsValid = EquipmentManager->CanSwapEquipment(FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType, ToEquipmentSlotType);
		}
		else
		{
			bIsValid = EquipmentManager->CanMoveOrMergeEquipment(FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType, ToEquipmentSlotType) > 0;
		}
	}
	
	if (bIsValid)
	{
		Image_Red->SetVisibility(ESlateVisibility::Hidden);
		Image_Green->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Image_Red->SetVisibility(ESlateVisibility::Visible);
		Image_Green->SetVisibility(ESlateVisibility::Hidden);
	}
	return true;
}

bool UD1EquipmentSlotSingleWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	OnDragEnded();

	UD1ItemDragDrop* ItemDragDrop = Cast<UD1ItemDragDrop>(InOperation);
	if (ItemDragDrop == nullptr)
		return false;

	if (UD1ItemEntryWidget* FromEntryWidget = ItemDragDrop->FromEntryWidget)
	{
		FromEntryWidget->RefreshWidgetOpacity(true);
	}

	UD1ItemInstance* FromItemInstance = ItemDragDrop->FromItemInstance;
	if (FromItemInstance == nullptr)
		return false;

	UD1ItemManagerComponent* ItemManager = GetOwningPlayer()->FindComponentByClass<UD1ItemManagerComponent>();
	if (ItemManager == nullptr)
		return false;

	EEquipmentSlotType ToEquipmentSlotType = GetEquipmentSlotType();
	if (ToEquipmentSlotType == EEquipmentSlotType::Count)
		return false;

	const UD1ItemFragment* FromItemFragment = nullptr;
	switch (EquipmentSingleSlotType)
	{
	case EEquipmentSingleSlotType::Armor:	FromItemFragment = FromItemInstance->FindFragmentByClass<UD1ItemFragment_Equipable_Armor>();	break;
	case EEquipmentSingleSlotType::Utility:	FromItemFragment = FromItemInstance->FindFragmentByClass<UD1ItemFragment_Equipable_Utility>();	break;
	}
	if (FromItemFragment == nullptr)
		return false;
	
	if (UD1InventoryManagerComponent* FromInventoryManager = ItemDragDrop->FromInventoryManager)
	{
		ItemManager->Server_InventoryToEquipment(FromInventoryManager, ItemDragDrop->FromItemSlotPos, EquipmentManager, ToEquipmentSlotType);
	}
	else if (UD1EquipmentManagerComponent* FromEquipmentManager = ItemDragDrop->FromEquipmentManager)
	{
		ItemManager->Server_EquipmentToEquipment(FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType, EquipmentManager, ToEquipmentSlotType);
	}
	
	return true;
}

void UD1EquipmentSlotSingleWidget::OnDragEnded()
{
	Super::OnDragEnded();

	Image_Red->SetVisibility(ESlateVisibility::Hidden);
	Image_Green->SetVisibility(ESlateVisibility::Hidden);
}

void UD1EquipmentSlotSingleWidget::OnEquipmentEntryChanged(UD1ItemInstance* InItemInstance, int32 InItemCount)
{
	if (EntryWidget)
	{
		Overlay_Entry->RemoveChild(EntryWidget);
		EntryWidget = nullptr;
	}
	
	if (InItemInstance)
	{
		TSubclassOf<UD1EquipmentEntryWidget> EntryWidgetClass = UD1UIData::Get().EquipmentEntryWidgetClass;
		EntryWidget = CreateWidget<UD1EquipmentEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
		
		UOverlaySlot* OverlaySlot = Overlay_Entry->AddChildToOverlay(EntryWidget);
		OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
		OverlaySlot->SetVerticalAlignment(VAlign_Fill);
		
		EntryWidget->Init(InItemInstance, InItemCount, GetEquipmentSlotType(), EquipmentManager);

		Image_BaseIcon->SetRenderOpacity(0.f);
	}
	else
	{
		Image_BaseIcon->SetRenderOpacity(1.f);
	}
}

EEquipmentSlotType UD1EquipmentSlotSingleWidget::GetEquipmentSlotType() const
{
	switch (EquipmentSingleSlotType)
	{
	case EEquipmentSingleSlotType::Armor:	return UD1EquipManagerComponent::ConvertToEquipmentSlotType(ArmorType);
	case EEquipmentSingleSlotType::Utility:	return UD1EquipManagerComponent::ConvertToEquipmentSlotType(UtilitySlotType);
	}

	return EEquipmentSlotType::Count;
}
