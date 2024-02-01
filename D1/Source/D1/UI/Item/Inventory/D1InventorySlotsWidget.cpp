#include "D1InventorySlotsWidget.h"

#include "D1InventoryEntryWidget.h"
#include "D1InventorySlotWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/UniformGridPanel.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemInstance.h"
#include "Item/Managers/D1InventoryManagerComponent.h"
#include "System/D1AssetManager.h"
#include "UI/Item/Drag/D1ItemDragDrop.h"
#include "UI/Item/Equipment/D1EquipmentEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventorySlotsWidget)

UD1InventorySlotsWidget::UD1InventorySlotsWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1InventorySlotsWidget::Init(UD1InventoryManagerComponent* InInventoryManagerComponent)
{
	check(InInventoryManagerComponent);
	InventoryManagerComponent = InInventoryManagerComponent;

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
		if (Entry.GetItemInstance())
		{
			FIntPoint ItemSlotPos = FIntPoint(i % InventorySlotCount.X, i / InventorySlotCount.X);
			OnInventoryEntryChanged(ItemSlotPos, Entry.GetItemInstance(), Entry.GetItemInstance()->GetItemID(), Entry.GetLastObservedCount(), Entry.GetItemCount());
		}
	}
	InventoryManagerComponent->OnInventoryEntryChanged.AddUObject(this, &ThisClass::OnInventoryEntryChanged);
}

void UD1InventorySlotsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SlotWidgetClass = UD1AssetManager::GetSubclassByName<UD1InventorySlotWidget>("InventorySlotWidget");
	EntryWidgetClass = UD1AssetManager::GetSubclassByName<UD1InventoryEntryWidget>("InventoryEntryWidget");
}

bool UD1InventorySlotsWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
	
	UD1ItemDragDrop* DragDrop = Cast<UD1ItemDragDrop>(InOperation);
	
	FVector2D MouseWidgetPos = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
	FVector2D ToWidgetPos = MouseWidgetPos - DragDrop->DeltaWidgetPos;
	const FVector2D& UnitSlotSize = UD1InventorySlotWidget::UnitSlotSize;
	FIntPoint ToSlotPos = FIntPoint(ToWidgetPos.X / UnitSlotSize.X, ToWidgetPos.Y / UnitSlotSize.Y);

	if (PrevDragOverSlotPos == ToSlotPos)
		return true;
	
	PrevDragOverSlotPos = ToSlotPos;

	UD1ItemInstance* FromItemInstance = nullptr;
	if (DragDrop->FromInventoryManager)
	{
		UD1InventoryEntryWidget* FromEntryWidget = DragDrop->InventoryEntryWidget;
		FromItemInstance = FromEntryWidget->GetItemInstance();
	}
	else if (DragDrop->FromEquipmentManager)
	{
		UD1EquipmentEntryWidget* FromEntryWidget = DragDrop->EquipmentEntryWidget;
		FromItemInstance = FromEntryWidget->GetItemInstance();
	}
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
	
	UnHoverSlots();

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
				SlotWidget->ChangeHoverState((MovableCount > 0) ? ESlotState::Valid : ESlotState::InValid);
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

	FVector2D MouseWidgetPos = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
	FVector2D ToWidgetPos = MouseWidgetPos - DragDrop->DeltaWidgetPos;
	const FVector2D& UnitSlotSize = UD1InventorySlotWidget::UnitSlotSize;
	FIntPoint ToSlotPos = FIntPoint(ToWidgetPos.X / UnitSlotSize.X, ToWidgetPos.Y / UnitSlotSize.Y);

	if (DragDrop->FromInventoryManager)
	{
		UD1InventoryEntryWidget* FromEntryWidget = DragDrop->InventoryEntryWidget;
		FromEntryWidget->RefreshWidgetOpacity(true);

		if (DragDrop->FromInventoryManager == InventoryManagerComponent)
		{
			InventoryManagerComponent->Server_RequestMoveOrMergeItem_FromInternalInventory(DragDrop->FromItemSlotPos, ToSlotPos);
		}
		else
		{
			InventoryManagerComponent->Server_RequestMoveOrMergeItem_FromExternalInventory(DragDrop->FromInventoryManager, DragDrop->FromItemSlotPos, ToSlotPos);
		}
	}
	else if (DragDrop->FromEquipmentManager)
	{
		UD1EquipmentEntryWidget* FromEntryWidget = DragDrop->EquipmentEntryWidget;
		FromEntryWidget->RefreshWidgetOpacity(true);
		InventoryManagerComponent->Server_RequestMoveOrMergeItem_FromExternalEquipment(DragDrop->FromEquipmentManager, DragDrop->FromEquipmentSlotType, ToSlotPos);
	}
	return true;
}

void UD1InventorySlotsWidget::UnHoverSlots()
{
	for (UD1InventorySlotWidget* SlotWidget : HoveredSlotWidgets)
	{
		SlotWidget->ChangeHoverState(ESlotState::Default);
	}
	HoveredSlotWidgets.Reset();
}

void UD1InventorySlotsWidget::FinishDrag()
{
	UnHoverSlots();
	PrevDragOverSlotPos = FIntPoint(-1, -1);
}

void UD1InventorySlotsWidget::OnInventoryEntryChanged_Implementation(const FIntPoint& InItemSlotPos, UD1ItemInstance* InItemInstance, int32 ItemID, int32 OldItemCount, int32 NewItemCount)
{
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();

	const FIntPoint& InventorySlotCount = InventoryManagerComponent->GetInventorySlotCount();
	
	int32 Index = InItemSlotPos.Y * InventorySlotCount.X + InItemSlotPos.X;
	if (UD1InventoryEntryWidget* EntryWidget = EntryWidgets[Index])
	{
		if (NewItemCount == 0)
		{
			if (UD1ItemInstance* ItemInstance = EntryWidget->GetItemInstance())
			{
				const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemInstance->GetItemID());
				const FIntPoint& ItemSlotCount = ItemDef.ItemSlotCount;

				const FIntPoint StartSlotPos = InItemSlotPos;
				const FIntPoint EndSlotPos = InItemSlotPos + ItemSlotCount;
				
				for (int y = StartSlotPos.Y; y < EndSlotPos.Y; y++)
				{
					for (int x = StartSlotPos.X; x < EndSlotPos.X; x++)
					{
						if (UD1InventorySlotWidget* SlotWidget = SlotWidgets[y * InventorySlotCount.X + x])
						{
							SlotWidget->ChangeSlotState(ESlotState::Default);
						}
					}
				}
			}
			
			CanvasPanel_Entries->RemoveChild(EntryWidget);
			EntryWidgets[Index] = nullptr;
		}
		else
		{
			EntryWidget->RefreshItemCount(NewItemCount);
		}
	}
	else if (NewItemCount > 0)
	{
		const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(InItemInstance->GetItemID());
		const FIntPoint& ItemSlotCount = ItemDef.ItemSlotCount;
		const FVector2D& UnitSlotSize = UD1InventorySlotWidget::UnitSlotSize;
			
		EntryWidget = CreateWidget<UD1InventoryEntryWidget>(GetWorld(), EntryWidgetClass);
		EntryWidgets[Index] = EntryWidget;

		UCanvasPanelSlot* EntryWidgetSlot = CanvasPanel_Entries->AddChildToCanvas(EntryWidget);
		EntryWidgetSlot->SetAutoSize(true);
		EntryWidgetSlot->SetPosition(FVector2D(InItemSlotPos.X * UnitSlotSize.X, InItemSlotPos.Y * UnitSlotSize.Y));
		
		FVector2D WidgetSize = FVector2D(ItemSlotCount.X * UnitSlotSize.X, ItemSlotCount.Y * UnitSlotSize.Y);
		EntryWidget->Init(this, WidgetSize, InItemInstance, NewItemCount);
		
		const FIntPoint StartSlotPos = InItemSlotPos;
		const FIntPoint EndSlotPos = InItemSlotPos + ItemSlotCount;
		
		for (int y = StartSlotPos.Y; y < EndSlotPos.Y; y++)
		{
			for (int x = StartSlotPos.X; x < EndSlotPos.X; x++)
			{
				if (UD1InventorySlotWidget* SlotWidget = SlotWidgets[y * InventorySlotCount.X + x])
				{
					SlotWidget->ChangeSlotState(ESlotState::Valid);
				}
			}
		}
	}
}
