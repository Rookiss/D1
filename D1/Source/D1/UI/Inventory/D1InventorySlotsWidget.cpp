#include "D1InventorySlotsWidget.h"

#include "D1InventoryDragDrop.h"
#include "D1InventoryEntryWidget.h"
#include "D1InventorySlotWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
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

	check(SlotWidgetClass);
	
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
				SlotSize = SlotWidget->SlotSize;
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

bool UD1InventorySlotsWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UD1InventorySlotsWidget::OnInventoryEntryChanged_Implementation(const FIntPoint& ItemPosition, UD1ItemInstance* ItemInstance, int32 ItemCount)
{
	int32 Index = ItemPosition.Y * InventorySlotCount.X + ItemPosition.X;
	if (EntryWidgets.IsValidIndex(Index) == false)
		return;

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	check(ItemData);

	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemInstance->GetItemID());
	const FIntPoint& ItemSlotCount = ItemDef.ItemSlotCount;
	
	if (UD1InventoryEntryWidget* EntryWidget = EntryWidgets[Index])
	{
		if (ItemCount == 0)
		{
			CanvasPanel_Entries->RemoveChild(EntryWidget);
			EntryWidgets[Index] = nullptr;

			for (int y = ItemPosition.Y; y < ItemPosition.Y + ItemSlotCount.Y; y++)
			{
				for (int x = ItemPosition.X; x < ItemPosition.X + ItemSlotCount.X; x++)
				{
					if (UD1InventorySlotWidget* SlotWidget = SlotWidgets[y * InventorySlotCount.X + x])
					{
						SlotWidget->SetEntryWidget(nullptr);
						SlotWidget->ChangeSlotColor(ESlotColor::Normal);
					}
				}
			}
		}
		else
		{
			EntryWidget->RefreshItemCount(ItemCount);
		}
	}
	else
	{
		if (ItemCount > 0)
		{
			UD1InventoryEntryWidget* NewEntryWidget = CreateWidget<UD1InventoryEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
			NewEntryWidget->Init(this, FVector2D(ItemSlotCount.X * SlotSize.X, ItemSlotCount.Y * SlotSize.Y), ItemInstance);
			NewEntryWidget->RefreshItemCount(ItemCount);
			EntryWidgets[Index] = NewEntryWidget;

			UCanvasPanelSlot* EntrySlot = CanvasPanel_Entries->AddChildToCanvas(NewEntryWidget);
			EntrySlot->SetAutoSize(true);
			EntrySlot->SetPosition(FVector2D(ItemPosition.X * SlotSize.X, ItemPosition.Y * SlotSize.Y));

			for (int y = ItemPosition.Y; y < ItemPosition.Y + ItemSlotCount.Y; y++)
			{
				for (int x = ItemPosition.X; x < ItemPosition.X + ItemSlotCount.X; x++)
				{
					if (UD1InventorySlotWidget* SlotWidget = SlotWidgets[y * InventorySlotCount.X + x])
					{
						SlotWidget->SetEntryWidget(NewEntryWidget);
						SlotWidget->ChangeSlotColor(ESlotColor::Blue);
					}
				}
			}
		}
	}
}
