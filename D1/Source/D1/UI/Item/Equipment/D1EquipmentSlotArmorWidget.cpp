#include "D1EquipmentSlotArmorWidget.h"

#include "D1EquipmentSlotsWidget.h"
#include "Character/D1Player.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/SizeBox.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equippable_Armor.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Item/Managers/D1InventoryManagerComponent.h"
#include "System/D1AssetManager.h"
#include "UI/Item/Drag/D1ItemDragDrop.h"
#include "UI/Item/Equipment/D1EquipmentEntryWidget.h"
#include "UI/Item/Inventory/D1InventoryEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentSlotArmorWidget)

UD1EquipmentSlotArmorWidget::UD1EquipmentSlotArmorWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1EquipmentSlotArmorWidget::Init(EEquipmentSlotType InEquipmentSlotType)
{
	check(InEquipmentSlotType != EEquipmentSlotType::EquipmentSlotCount);
	EquipmentSlotType = InEquipmentSlotType;
}

void UD1EquipmentSlotArmorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AD1Player* Player = Cast<AD1Player>(GetOwningPlayerPawn());
	check(Player);

	EquipmentManagerComponent = Player->EquipmentManagerComponent;
	check(EquipmentManagerComponent);
	
	EntryWidgetClass = UD1AssetManager::GetSubclassByName<UD1EquipmentEntryWidget>("EquipmentEntryWidget");
}

bool UD1EquipmentSlotArmorWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
	
	if (bAlreadyHovered)
		return true;

	bAlreadyHovered = true;
	
	UD1ItemDragDrop* DragDrop = Cast<UD1ItemDragDrop>(InOperation);
	check(DragDrop);

	UD1ItemInstance* FromItemInstance = DragDrop->ItemInstance;
	check(FromItemInstance);
	
	bool bIsValid = false;

	if (FromItemInstance->FindFragmentByClass<UD1ItemFragment_Equippable_Armor>())
	{
		if (UD1InventoryManagerComponent* InventoryManager = DragDrop->FromInventoryManager)
		{
			bIsValid = EquipmentManagerComponent->CanEquipItem_FromInventory(InventoryManager, DragDrop->FromItemSlotPos, EquipmentSlotType);
		}
		else if (UD1EquipmentManagerComponent* EquipmentManager = DragDrop->FromEquipmentManager)
		{
			bIsValid = EquipmentManagerComponent->CanEquipItem_FromEquipment(EquipmentManager, DragDrop->FromEquipmentSlotType, EquipmentSlotType);
		}
	}
	
	ChangeHoverState(Image_Slot, bIsValid ? ESlotState::Valid : ESlotState::InValid);
	return true;
}

void UD1EquipmentSlotArmorWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	FinishDrag();
}

bool UD1EquipmentSlotArmorWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
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

void UD1EquipmentSlotArmorWidget::FinishDrag()
{
	ChangeHoverState(Image_Slot, ESlotState::Default);
	bAlreadyHovered = false;
}

void UD1EquipmentSlotArmorWidget::OnEquipmentEntryChanged(UD1ItemInstance* InItemInstance)
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
		EntryWidget->Init(InItemInstance, EquipmentSlotType);
		ChangeSlotState(Image_Slot, ESlotState::Valid);
	}
	else
	{
		ChangeSlotState(Image_Slot, ESlotState::Default);
		Overlay_Root->RemoveChild(EntryWidget);
		EntryWidget = nullptr;
	}
}
