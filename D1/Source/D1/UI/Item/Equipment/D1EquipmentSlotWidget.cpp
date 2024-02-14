#include "D1EquipmentSlotWidget.h"

#include "D1EquipmentSlotsWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/SizeBox.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Item/Managers/D1InventoryManagerComponent.h"
#include "System/D1AssetManager.h"
#include "UI/Item/Drag/D1ItemDragDrop.h"
#include "UI/Item/Equipment/D1EquipmentEntryWidget.h"
#include "UI/Item/Inventory/D1InventoryEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentSlotWidget)

UD1EquipmentSlotWidget::UD1EquipmentSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1EquipmentSlotWidget::Init(UD1EquipmentManagerComponent* InEquipmentManagerComponent, EEquipmentSlotType InEquipmentSlotType)
{
	check(InEquipmentManagerComponent);
	EquipmentManagerComponent = InEquipmentManagerComponent;

	check(InEquipmentSlotType != EEquipmentSlotType::EquipmentSlotCount);
	EquipmentSlotType = InEquipmentSlotType;
}

void UD1EquipmentSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	EntryWidgetClass = UD1AssetManager::GetSubclassByName<UD1EquipmentEntryWidget>("EquipmentEntryWidget");
}

bool UD1EquipmentSlotWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
	
	if (bAlreadyHovered)
		return true;

	bAlreadyHovered = true;
	
	UD1ItemDragDrop* DragDrop = Cast<UD1ItemDragDrop>(InOperation);
	
	bool bIsValid = false;
	
	if (UD1InventoryManagerComponent* InventoryManager = DragDrop->FromInventoryManager)
	{
		bIsValid = EquipmentManagerComponent->CanEquipItem_FromInventory(InventoryManager, DragDrop->FromItemSlotPos, EquipmentSlotType);
	}
	else if (UD1EquipmentManagerComponent* EquipmentManager = DragDrop->FromEquipmentManager)
	{
		bIsValid = EquipmentManagerComponent->CanEquipItem_FromEquipment(EquipmentManager, DragDrop->FromEquipmentSlotType, EquipmentSlotType);
	}
	
	ChangeHoverState(bIsValid ? ESlotState::Valid : ESlotState::InValid);
	return true;
}

void UD1EquipmentSlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	FinishDrag();
}

bool UD1EquipmentSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	FinishDrag();

	UD1ItemDragDrop* DragDrop = Cast<UD1ItemDragDrop>(InOperation);
	
	if (UD1InventoryManagerComponent* InventoryManager = DragDrop->FromInventoryManager)
	{
		UD1InventoryEntryWidget* InventoryEntryWidget = DragDrop->InventoryEntryWidget;
		InventoryEntryWidget->RefreshWidgetOpacity(true);
		EquipmentManagerComponent->Server_RequestEquipItem_FromInventory(InventoryManager, DragDrop->FromItemSlotPos, EquipmentSlotType);
	}
	else if (UD1EquipmentManagerComponent* EquipmentManager = DragDrop->FromEquipmentManager)
	{
		UD1EquipmentEntryWidget* EquipmentEntryWidget = DragDrop->EquipmentEntryWidget;
		EquipmentEntryWidget->RefreshWidgetOpacity(true);
		EquipmentManagerComponent->Server_RequestEquipItem_FromEquipment(EquipmentManager, DragDrop->FromEquipmentSlotType, EquipmentSlotType);
	}
	return true;
}

void UD1EquipmentSlotWidget::FinishDrag()
{
	ChangeHoverState(ESlotState::Default);
	bAlreadyHovered = false;
}

void UD1EquipmentSlotWidget::OnEquipmentEntryChanged_Implementation(UD1ItemInstance* InItemInstance)
{
	if (InItemInstance)
	{
		if (EntryWidget)
		{
			Overlay_Root->RemoveChild(EntryWidget);
			EntryWidget = nullptr;
		}
		
		EntryWidget = CreateWidget<UD1EquipmentEntryWidget>(GetWorld(), EntryWidgetClass);
		Overlay_Root->AddChildToOverlay(EntryWidget);

		UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot);
		EntryWidget->Init(EquipmentManagerComponent, CanvasPanelSlot->GetSize(), InItemInstance, EquipmentSlotType);
		ChangeSlotState(ESlotState::Valid);
	}
	else
	{
		ChangeSlotState(ESlotState::Default);
		Overlay_Root->RemoveChild(EntryWidget);
		EntryWidget = nullptr;
	}
}
