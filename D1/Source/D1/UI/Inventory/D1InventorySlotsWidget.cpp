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

void UD1InventorySlotsWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	check(SlotWidgetClass);

	SlotWidgets.Reserve(SlotCount.X * SlotCount.Y);
	
	for (int32 Y = 0; Y < SlotCount.Y; Y++)
	{
		for (int32 X = 0; X < SlotCount.X; X++)
		{
			UD1InventorySlotWidget* SlotWidget = CreateWidget<UD1InventorySlotWidget>(GetWorld(), SlotWidgetClass);
			SlotWidgets.Add(SlotWidget);
			GridPanel_Slots->AddChildToUniformGrid(SlotWidget, Y, X);
		}
	}
}

void UD1InventorySlotsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(SlotWidgetClass);

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
	
	if (AD1PlayerController* PC = Cast<AD1PlayerController>(GetOwningPlayer()))
	{
		if (UD1InventoryManagerComponent* InventoryManager = PC->InventoryManagerComponent)
		{
			for (const FD1InventoryEntry& Entry : InventoryManager->GetAllItems())
			{
				OnInventoryEntryChanged(Entry.GetPosition(), Entry.GetInstance(), Entry.GetCount());
			}
			InventoryManager->OnInventoryEntryChanged.AddUObject(this, &ThisClass::OnInventoryEntryChanged);
		}
	}
}

void UD1InventorySlotsWidget::OnInventoryEntryChanged_Implementation(const FIntPoint& Position, UD1ItemInstance* Instance, int32 ItemCount)
{
	
}
