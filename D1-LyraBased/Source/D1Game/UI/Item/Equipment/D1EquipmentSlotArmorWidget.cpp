#include "D1EquipmentSlotArmorWidget.h"

#include "D1EquipmentSlotsWidget.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Armor.h"
#include "Item/Managers/D1EquipManagerComponent.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Item/Managers/D1InventoryManagerComponent.h"
#include "Item/Managers/D1ItemManagerComponent.h"
#include "UI/Item/D1ItemDragDrop.h"
#include "UI/Item/Equipment/D1EquipmentEntryWidget.h"
#include "UI/Item/Inventory/D1InventoryEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentSlotArmorWidget)

UD1EquipmentSlotArmorWidget::UD1EquipmentSlotArmorWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1EquipmentSlotArmorWidget::Init(EArmorType InArmorType, UD1EquipmentManagerComponent* InEquipmentManager)
{
	check(InArmorType != EArmorType::Count && InEquipmentManager != nullptr);
	
	ArmorType = InArmorType;
	EquipmentManager = InEquipmentManager;
}

void UD1EquipmentSlotArmorWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	Image_Icon->SetBrushFromTexture(ArmorIconTexture, true);
}

bool UD1EquipmentSlotArmorWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
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
	EEquipmentSlotType ToEquipmentSlotType = UD1EquipManagerComponent::ConvertToEquipmentSlotType(ArmorType);
	
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
	
	ChangeHoverState(Image_Slot, bIsValid ? ESlotState::Valid : ESlotState::Invalid);
	return true;
}

bool UD1EquipmentSlotArmorWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	FinishDrag();

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
	
	if (FromItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Armor>())
	{
		EEquipmentSlotType ToEquipmentSlotType = UD1EquipManagerComponent::ConvertToEquipmentSlotType(ArmorType);
	
		if (UD1InventoryManagerComponent* FromInventoryManager = ItemDragDrop->FromInventoryManager)
		{
			ItemManager->Server_InventoryToEquipment(FromInventoryManager, ItemDragDrop->FromItemSlotPos, EquipmentManager, ToEquipmentSlotType);
		}
		else if (UD1EquipmentManagerComponent* FromEquipmentManager = ItemDragDrop->FromEquipmentManager)
		{
			ItemManager->Server_EquipmentToEquipment(FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType, EquipmentManager, ToEquipmentSlotType);
		}
	}
	return true;
}

void UD1EquipmentSlotArmorWidget::FinishDrag()
{
	Super::FinishDrag();
	
	ChangeHoverState(Image_Slot, ESlotState::Default);
}

void UD1EquipmentSlotArmorWidget::OnEquipmentEntryChanged(UD1ItemInstance* InItemInstance, int32 InItemCount)
{
	if (EntryWidget)
	{
		Overlay_Root->RemoveChild(EntryWidget);
		EntryWidget = nullptr;
	}
	
	if (InItemInstance)
	{
		EntryWidget = CreateWidget<UD1EquipmentEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
		
		UOverlaySlot* OverlaySlot = Overlay_Root->AddChildToOverlay(EntryWidget);
		OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
		OverlaySlot->SetVerticalAlignment(VAlign_Fill);
		
		EntryWidget->Init(InItemInstance, InItemCount, UD1EquipManagerComponent::ConvertToEquipmentSlotType(ArmorType), EquipmentManager);

		Image_Icon->SetRenderOpacity(0.f);
	}
	else
	{
		Image_Icon->SetRenderOpacity(1.f);
	}
}
