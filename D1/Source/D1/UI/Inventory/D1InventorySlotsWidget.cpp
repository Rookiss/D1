#include "D1InventorySlotsWidget.h"

#include "D1InventoryDragDrop.h"
#include "D1InventoryEntryWidget.h"
#include "D1InventorySlotWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/UniformGridPanel.h"
#include "Data/D1ItemData.h"
#include "Inventory/D1InventoryManagerComponent.h"
#include "Inventory/D1ItemInstance.h"
#include "Player/D1PlayerController.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventorySlotsWidget)

UD1InventorySlotsWidget::UD1InventorySlotsWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1InventorySlotsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SlotWidgetClass = UD1AssetManager::GetSubclassByName<UD1InventorySlotWidget>("SlotWidget");
	EntryWidgetClass = UD1AssetManager::GetSubclassByName<UD1InventoryEntryWidget>("EntryWidget");
	
	if (AD1PlayerController* PC = Cast<AD1PlayerController>(GetOwningPlayer()))
	{
		if (UD1InventoryManagerComponent* InventoryManager = PC->InventoryManagerComponent)
		{
			InventoryManagerComponent = InventoryManager;

			InventorySlotCount = InventoryManagerComponent->GetInventorySlotCount();
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

			if (UD1InventorySlotWidget* SlotWidget = Cast<UD1InventorySlotWidget>(GridPanel_Slots->GetChildAt(0)))
			{
				UnitSlotSize = SlotWidget->GetSlotSize();
			}

			const TArray<FD1InventoryEntry>& Entries = InventoryManagerComponent->GetAllItems();
			for (int32 i = 0; i < Entries.Num(); i++)
			{
				const FD1InventoryEntry& Entry = Entries[i];
				if (Entry.GetItemInstance())
				{
					FIntPoint ItemPosition = FIntPoint(i % InventorySlotCount.X, i / InventorySlotCount.X);
					OnInventoryEntryChanged(ItemPosition, Entry.GetItemInstance(), Entry.GetItemCount());
				}
			}
			InventoryManagerComponent->OnInventoryEntryChanged.AddUObject(this, &ThisClass::OnInventoryEntryChanged);
		}
	}
}

bool UD1InventorySlotsWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	// @TODO: Hover Slot Color
	
	
	return true;
}

bool UD1InventorySlotsWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UD1InventoryDragDrop* DragDrop = Cast<UD1InventoryDragDrop>(InOperation);
	check(DragDrop);

	if (UD1InventoryEntryWidget* EntryWidget = DragDrop->EntryWidget)
	{
		EntryWidget->RefreshRenderOpacity(true);
	}
	
	FVector2D MousePosition = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
	FIntPoint ToPosition = FIntPoint(MousePosition.X / UnitSlotSize.X, MousePosition.Y / UnitSlotSize.Y) - DragDrop->DeltaPosition;
	InventoryManagerComponent->RequestMoveItem(DragDrop->FromPosition, ToPosition);
	return true;
}

void UD1InventorySlotsWidget::OnInventoryEntryChanged_Implementation(const FIntPoint& InItemPosition, UD1ItemInstance* InItemInstance, int32 InItemCount)
{
	int32 Index = InItemPosition.Y * InventorySlotCount.X + InItemPosition.X;
	if (EntryWidgets.IsValidIndex(Index) == false)
		return;

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	check(ItemData);
	
	if (UD1InventoryEntryWidget* EntryWidget = EntryWidgets[Index])
	{
		if (InItemCount == 0)
		{
			if (UD1ItemInstance* ItemInstance = EntryWidget->GetItemInstance())
			{
				const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemInstance->GetItemID());
				const FIntPoint& ItemSlotCount = ItemDef.ItemSlotCount;

				for (int y = InItemPosition.Y; y < InItemPosition.Y + ItemSlotCount.Y; y++)
				{
					for (int x = InItemPosition.X; x < InItemPosition.X + ItemSlotCount.X; x++)
					{
						if (UD1InventorySlotWidget* SlotWidget = SlotWidgets[y * InventorySlotCount.X + x])
						{
							SlotWidget->SetEntryWidget(nullptr);
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
			EntryWidget->RefreshItemCount(InItemCount);
		}
	}
	else
	{
		if (InItemCount > 0)
		{
			const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(InItemInstance->GetItemID());
			const FIntPoint& ItemSlotCount = ItemDef.ItemSlotCount;
			
			UD1InventoryEntryWidget* NewEntryWidget = CreateWidget<UD1InventoryEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
			
			UCanvasPanelSlot* EntrySlot = CanvasPanel_Entries->AddChildToCanvas(NewEntryWidget);
			EntrySlot->SetAutoSize(true);
			EntrySlot->SetPosition(FVector2D(InItemPosition.X * UnitSlotSize.X, InItemPosition.Y * UnitSlotSize.Y));

			FVector2D WidgetSize = FVector2D(ItemSlotCount.X * UnitSlotSize.X, ItemSlotCount.Y * UnitSlotSize.Y);
			NewEntryWidget->Init(this, WidgetSize, InItemInstance, InItemCount);
			EntryWidgets[Index] = NewEntryWidget;

			for (int y = InItemPosition.Y; y < InItemPosition.Y + ItemSlotCount.Y; y++)
			{
				for (int x = InItemPosition.X; x < InItemPosition.X + ItemSlotCount.X; x++)
				{
					if (UD1InventorySlotWidget* SlotWidget = SlotWidgets[y * InventorySlotCount.X + x])
					{
						SlotWidget->SetEntryWidget(NewEntryWidget);
						SlotWidget->ChangeSlotState(ESlotState::Valid);
					}
				}
			}
		}
	}
}
