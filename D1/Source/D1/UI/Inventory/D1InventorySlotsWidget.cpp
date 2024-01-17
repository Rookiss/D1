#include "D1InventorySlotsWidget.h"

#include "D1InventorySlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Inventory/D1InventoryManagerComponent.h"
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
			SlotCount = InventoryManager->GetInventorySlotCount();
			SlotWidgets.Reserve(SlotCount.X * SlotCount.Y);
	
			for (int32 Y = 0; Y < SlotCount.Y; Y++)
			{
				for (int32 X = 0; X < SlotCount.X; X++)
				{
					UD1InventorySlotWidget* SlotWidget = CreateWidget<UD1InventorySlotWidget>(GetOwningPlayer(), SlotWidgetClass);
					SlotWidgets.Add(SlotWidget);
					GridPanel_Slots->AddChildToUniformGrid(SlotWidget, Y, X);
				}
			}

			const TArray<FD1InventoryEntry>& Entries = InventoryManager->GetAllItems();
			for (int32 i = 0; i < Entries.Num(); i++)
			{
				const FD1InventoryEntry& Entry = Entries[i];
				if (Entry.GetItemInstance())
				{
					const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();
					FIntPoint ItemPosition = FIntPoint(i % InventorySlotCount.X, i / InventorySlotCount.X);
					OnInventoryEntryChanged(ItemPosition, Entry.GetItemInstance(), Entry.GetItemCount());
				}
			}
			InventoryManager->OnInventoryEntryChanged.AddUObject(this, &ThisClass::OnInventoryEntryChanged);
		}
	}
}

void UD1InventorySlotsWidget::OnInventoryEntryChanged_Implementation(const FIntPoint& ItemPosition, UD1ItemInstance* ItemInstance, int32 ItemCount)
{
	
}
