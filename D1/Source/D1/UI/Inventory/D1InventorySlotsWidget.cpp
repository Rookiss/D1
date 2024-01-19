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
	
	if (AD1PlayerController* PlayerController = Cast<AD1PlayerController>(GetOwningPlayer()))
	{
		InventoryManagerComponent = PlayerController->InventoryManagerComponent;

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

		const TArray<FD1InventoryEntry>& Entries = InventoryManagerComponent->GetAllEntries();
		for (int32 i = 0; i < Entries.Num(); i++)
		{
			const FD1InventoryEntry& Entry = Entries[i];
			if (Entry.GetItemInstance())
			{
				FIntPoint ItemSlotPos = FIntPoint(i % InventorySlotCount.X, i / InventorySlotCount.X);
				OnInventoryEntryChanged(ItemSlotPos, Entry.GetItemInstance(), Entry.GetItemCount());
			}
		}
		InventoryManagerComponent->OnInventoryEntryChanged.AddUObject(this, &ThisClass::OnInventoryEntryChanged);
	}
}

// @TODO
bool UD1InventorySlotsWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
	
	UD1InventoryDragDrop* DragDrop = Cast<UD1InventoryDragDrop>(InOperation);
	
	FVector2D MouseWidgetPos = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
	FVector2D ToWidgetPos = MouseWidgetPos - DragDrop->DeltaWidgetPos;
	FIntPoint ToSlotPos = FIntPoint(ToWidgetPos.X / UnitSlotSize.X, ToWidgetPos.Y / UnitSlotSize.Y);

	if (PrevToSlotPos == ToSlotPos)
		return true;
	
	PrevToSlotPos = ToSlotPos;

	UD1InventoryEntryWidget* FromEntryWidget = DragDrop->EntryWidget;
	UD1ItemInstance* FromItemInstance = FromEntryWidget->GetItemInstance();

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FD1ItemDefinition& FromItemDef = ItemData->GetItemDefByID(FromItemInstance->GetItemID());
	const FIntPoint& FromItemSlotCount = FromItemDef.ItemSlotCount;

	const int32 FromItemID = FromItemInstance->GetItemID();
	const FIntPoint& FromSlotPos = DragDrop->FromSlotPos;
	
	int32 ToItemID = -1;
	if (ToSlotPos.X >= 0 && ToSlotPos.Y >= 0)
	{
		int32 Index = ToSlotPos.Y * InventorySlotCount.X + ToSlotPos.X;
		if (UD1InventoryEntryWidget* ToEntryWidget = EntryWidgets[Index])
		{
			UD1ItemInstance* ToItemInstance = ToEntryWidget->GetItemInstance();
			ToItemID = ToItemInstance->GetItemID();
		}
	}
	
	bool bIsValid = InventoryManagerComponent->CanMoveItem(FromItemID, FromSlotPos, ToSlotPos) ||
					InventoryManagerComponent->CanMergeItem(FromItemID, FromSlotPos, ToItemID, ToSlotPos);

	UnHoverSlots();
	
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
				SlotWidget->ChangeHoverState(bIsValid ? ESlotState::Valid : ESlotState::InValid);
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
	
	UD1InventoryDragDrop* DragDrop = Cast<UD1InventoryDragDrop>(InOperation);
	if (UD1InventoryEntryWidget* EntryWidget = DragDrop->EntryWidget)
	{
		EntryWidget->RefreshWidgetOpacity(true);
	}
	
	FVector2D MouseWidgetPos = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
	FVector2D ToWidgetPos = MouseWidgetPos - DragDrop->DeltaWidgetPos;
	FIntPoint ToSlotPos = FIntPoint(ToWidgetPos.X / UnitSlotSize.X, ToWidgetPos.Y / UnitSlotSize.Y);
	
	InventoryManagerComponent->RequestMoveOrMergeItem(DragDrop->FromSlotPos, ToSlotPos);
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
	PrevToSlotPos = FIntPoint(-1, -1);
}

void UD1InventorySlotsWidget::OnInventoryEntryChanged_Implementation(const FIntPoint& InItemSlotPos, UD1ItemInstance* InItemInstance, int32 InItemCount)
{
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();

	int32 Index = InItemSlotPos.Y * InventorySlotCount.X + InItemSlotPos.X;
	if (UD1InventoryEntryWidget* EntryWidget = EntryWidgets[Index])
	{
		if (InItemCount == 0)
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
			EntryWidget->RefreshItemCount(InItemCount);
		}
	}
	else if (InItemCount > 0)
	{
		const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(InItemInstance->GetItemID());
		const FIntPoint& ItemSlotCount = ItemDef.ItemSlotCount;
			
		EntryWidget = CreateWidget<UD1InventoryEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
		EntryWidgets[Index] = EntryWidget;

		UCanvasPanelSlot* EntryWidgetSlot = CanvasPanel_Entries->AddChildToCanvas(EntryWidget);
		EntryWidgetSlot->SetAutoSize(true);
		EntryWidgetSlot->SetPosition(FVector2D(InItemSlotPos.X * UnitSlotSize.X, InItemSlotPos.Y * UnitSlotSize.Y));
		
		FVector2D WidgetSize = FVector2D(ItemSlotCount.X * UnitSlotSize.X, ItemSlotCount.Y * UnitSlotSize.Y);
		EntryWidget->Init(this, WidgetSize, InItemInstance, InItemCount);
		
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
