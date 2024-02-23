#include "D1InventorySlotsWidget.h"

#include "D1InventoryEntryWidget.h"
#include "D1InventorySlotWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/UniformGridPanel.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemInstance.h"
#include "Item/Managers/D1InventoryManagerComponent.h"
#include "Player/D1PlayerController.h"
#include "System/D1AssetManager.h"
#include "UI/Item/Drag/D1ItemDragDrop.h"
#include "UI/Item/Equipment/D1EquipmentEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventorySlotsWidget)

UD1InventorySlotsWidget::UD1InventorySlotsWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1InventorySlotsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SlotWidgetClass = UD1AssetManager::GetSubclassByName<UD1InventorySlotWidget>("InventorySlotWidget");
	EntryWidgetClass = UD1AssetManager::GetSubclassByName<UD1InventoryEntryWidget>("InventoryEntryWidget");

	AD1PlayerController* PC = Cast<AD1PlayerController>(GetOwningPlayer());
	check(PC);

	InventoryManagerComponent = PC->InventoryManagerComponent;
	check(InventoryManagerComponent);
	
	const FIntPoint& InventorySlotCount = InventoryManagerComponent->GetInventorySlotCount();
	SlotWidgets.SetNum(InventorySlotCount.X * InventorySlotCount.Y);
	EntryWidgets.SetNum(InventorySlotCount.X * InventorySlotCount.Y);
			
	for (int32 y = 0; y < InventorySlotCount.Y; y++)
	{
		for (int32 x = 0; x < InventorySlotCount.X; x++)
		{
			UD1InventorySlotWidget* SlotWidget = CreateWidget<UD1InventorySlotWidget>(GetOwningPlayer(), SlotWidgetClass);
			SlotWidgets[y * InventorySlotCount.X + x] = SlotWidget;
			GridPanel_Slots->AddChildToUniformGrid(SlotWidget, y, x);
		}
	}

	const TArray<FD1InventoryEntry>& Entries = InventoryManagerComponent->GetAllEntries();
	for (int32 i = 0; i < Entries.Num(); i++)
	{
		const FD1InventoryEntry& Entry = Entries[i];
		if (UD1ItemInstance* ItemInstance = Entry.GetItemInstance())
		{
			FIntPoint ItemSlotPos = FIntPoint(i % InventorySlotCount.X, i / InventorySlotCount.X);
			OnInventoryEntryChanged(ItemSlotPos, ItemInstance, Entry.GetItemCount());
		}
	}
	InventoryManagerComponent->OnInventoryEntryChanged.AddUObject(this, &ThisClass::OnInventoryEntryChanged);
}

bool UD1InventorySlotsWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
	
	UD1ItemDragDrop* DragDrop = Cast<UD1ItemDragDrop>(InOperation);
	check(DragDrop);
	
	FVector2D MouseWidgetPos = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
	FVector2D ToWidgetPos = MouseWidgetPos - DragDrop->DeltaWidgetPos;
	FIntPoint ToSlotPos = FIntPoint(ToWidgetPos.X / Item::UnitInventorySlotSize.X, ToWidgetPos.Y / Item::UnitInventorySlotSize.Y);

	if (PrevDragOverSlotPos == ToSlotPos)
		return true;
	
	PrevDragOverSlotPos = ToSlotPos;
	
	UD1ItemEntryWidget* FromEntryWidget = DragDrop->FromEntryWidget;
	UD1ItemInstance* FromItemInstance = FromEntryWidget->GetItemInstance();
	check(FromItemInstance);

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FD1ItemDefinition& FromItemDef = ItemData->GetItemDefByID(FromItemInstance->GetItemID());
	const FIntPoint& FromItemSlotCount = FromItemDef.ItemSlotCount;

	int32 MovableCount = 0;
	if (DragDrop->FromInventoryManager)
	{
		if (DragDrop->FromInventoryManager == InventoryManagerComponent)
		{
			MovableCount = InventoryManagerComponent->CanMoveOrMergeItem_FromInternalInventory(DragDrop->FromItemSlotPos, ToSlotPos);
		}
		else
		{
			MovableCount = InventoryManagerComponent->CanMoveOrMergeItem_FromExternalInventory(DragDrop->FromInventoryManager, DragDrop->FromItemSlotPos, ToSlotPos);
		}
	}
	else if (DragDrop->FromEquipmentManager)
	{
		MovableCount = InventoryManagerComponent->CanMoveOrMergeItem_FromExternalEquipment(DragDrop->FromEquipmentManager, DragDrop->FromEquipmentSlotType, ToSlotPos);
	}
	
	UnhoverSlots();

	const FIntPoint& InventorySlotCount = InventoryManagerComponent->GetInventorySlotCount();
	
	const FIntPoint StartSlotPos = FIntPoint(FMath::Max(0, ToSlotPos.X), FMath::Max(0, ToSlotPos.Y));
	const FIntPoint EndSlotPos   = FIntPoint(FMath::Min(ToSlotPos.X + FromItemSlotCount.X, InventorySlotCount.X),
											 FMath::Min(ToSlotPos.Y + FromItemSlotCount.Y, InventorySlotCount.Y));
	
	for (int32 y = StartSlotPos.Y; y < EndSlotPos.Y; y++)
	{
		for (int32 x = StartSlotPos.X; x < EndSlotPos.X; x++)
		{
			int32 Index = y * InventorySlotCount.X + x;
			if (UD1InventorySlotWidget* SlotWidget = SlotWidgets[Index])
			{
				HoveredSlotWidgets.Add(SlotWidget);
				SlotWidget->ChangeHoverState(SlotWidget->Image_Slot, (MovableCount > 0) ? ESlotState::Valid : ESlotState::Invalid);
			}
		}
	}
	return true;
}

void UD1InventorySlotsWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	FinishDrag();
}

bool UD1InventorySlotsWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	FinishDrag();
	
	UD1ItemDragDrop* DragDrop = Cast<UD1ItemDragDrop>(InOperation);
	check(DragDrop);

	UD1ItemEntryWidget* FromEntryWidget = DragDrop->FromEntryWidget;
	FromEntryWidget->RefreshWidgetOpacity(true);
	
	FVector2D MouseWidgetPos = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
	FVector2D ToWidgetPos = MouseWidgetPos - DragDrop->DeltaWidgetPos;
	FIntPoint ToItemSlotPos = FIntPoint(ToWidgetPos.X / Item::UnitInventorySlotSize.X, ToWidgetPos.Y / Item::UnitInventorySlotSize.Y);

	if (DragDrop->FromInventoryManager)
	{
		if (DragDrop->FromInventoryManager == InventoryManagerComponent)
		{
			InventoryManagerComponent->Server_RequestMoveOrMergeItem_FromInternalInventory(DragDrop->FromItemSlotPos, ToItemSlotPos);
		}
		else
		{
			InventoryManagerComponent->Server_RequestMoveOrMergeItem_FromExternalInventory(DragDrop->FromInventoryManager, DragDrop->FromItemSlotPos, ToItemSlotPos);
		}
	}
	else if (DragDrop->FromEquipmentManager)
	{
		InventoryManagerComponent->Server_RequestMoveOrMergeItem_FromExternalEquipment(DragDrop->FromEquipmentManager, DragDrop->FromEquipmentSlotType, ToItemSlotPos);
	}
	
	return true;
}

void UD1InventorySlotsWidget::UnhoverSlots()
{
	for (UD1InventorySlotWidget* SlotWidget : HoveredSlotWidgets)
	{
		SlotWidget->ChangeHoverState(SlotWidget->Image_Slot, ESlotState::Default);
	}
	HoveredSlotWidgets.Reset();
}

void UD1InventorySlotsWidget::FinishDrag()
{
	UnhoverSlots();
	PrevDragOverSlotPos = FIntPoint(-1, -1);
}

void UD1InventorySlotsWidget::OnInventoryEntryChanged(const FIntPoint& InItemSlotPos, UD1ItemInstance* InItemInstance, int32 NewItemCount)
{
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FIntPoint& InventorySlotCount = InventoryManagerComponent->GetInventorySlotCount();
	int32 SlotIndex = InItemSlotPos.Y * InventorySlotCount.X + InItemSlotPos.X;
	
	if (UD1InventoryEntryWidget* EntryWidget = EntryWidgets[SlotIndex])
	{
		if (NewItemCount == 0)
		{
			if (UD1ItemInstance* ItemInstance = EntryWidget->GetItemInstance())
			{
				const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemInstance->GetItemID());

				const FIntPoint StartSlotPos = InItemSlotPos;
				const FIntPoint EndSlotPos = InItemSlotPos + ItemDef.ItemSlotCount;
				
				for (int y = StartSlotPos.Y; y < EndSlotPos.Y; y++)
				{
					for (int x = StartSlotPos.X; x < EndSlotPos.X; x++)
					{
						if (UD1InventorySlotWidget* SlotWidget = SlotWidgets[y * InventorySlotCount.X + x])
						{
							SlotWidget->ChangeSlotState(SlotWidget->Image_Slot, ESlotState::Default);
						}
					}
				}
			}
			
			CanvasPanel_Entries->RemoveChild(EntryWidget);
			EntryWidgets[SlotIndex] = nullptr;
		}
		else
		{
			EntryWidget->RefreshItemCount(NewItemCount);
		}
	}
	else if (NewItemCount > 0)
	{
		const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(InItemInstance->GetItemID());
		
		EntryWidget = CreateWidget<UD1InventoryEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
		EntryWidgets[SlotIndex] = EntryWidget;

		UCanvasPanelSlot* CanvasPanelSlot = CanvasPanel_Entries->AddChildToCanvas(EntryWidget);
		CanvasPanelSlot->SetAutoSize(true);
		CanvasPanelSlot->SetPosition(FVector2D(InItemSlotPos.X * Item::UnitInventorySlotSize.X, InItemSlotPos.Y * Item::UnitInventorySlotSize.Y));
		
		EntryWidget->Init(this, InItemInstance, NewItemCount);
		
		const FIntPoint StartSlotPos = InItemSlotPos;
		const FIntPoint EndSlotPos = InItemSlotPos + ItemDef.ItemSlotCount;
		
		for (int y = StartSlotPos.Y; y < EndSlotPos.Y; y++)
		{
			for (int x = StartSlotPos.X; x < EndSlotPos.X; x++)
			{
				if (UD1InventorySlotWidget* SlotWidget = SlotWidgets[y * InventorySlotCount.X + x])
				{
					SlotWidget->ChangeSlotState(SlotWidget->Image_Slot, ESlotState::Valid);
				}
			}
		}
	}
}
