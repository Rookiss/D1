#include "D1InventorySlotsWidget.h"

#include "AbilitySystemComponent.h"
#include "D1InventoryEntryWidget.h"
#include "D1InventorySlotWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/UniformGridPanel.h"
#include "Data/D1ItemData.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Item/D1ItemInstance.h"
#include "Item/D1ItemTemplate.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Item/Managers/D1InventoryManagerComponent.h"
#include "Item/Managers/D1ItemManagerComponent.h"
#include "System/LyraAssetManager.h"
#include "UI/Item/D1ItemDragDrop.h"
#include "UI/Item/Equipment/D1EquipmentEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventorySlotsWidget)

UD1InventorySlotsWidget::UD1InventorySlotsWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1InventorySlotsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	ListenerHandle = MessageSubsystem.RegisterListener(MessageChannelTag, this, &ThisClass::ConstructUI);
}

void UD1InventorySlotsWidget::NativeDestruct()
{
	DestructUI();
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(ListenerHandle);
	
	Super::NativeDestruct();
}

bool UD1InventorySlotsWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
	
	UD1ItemDragDrop* DragDrop = Cast<UD1ItemDragDrop>(InOperation);
	check(DragDrop);
	
	FVector2D MouseWidgetPos = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
	FVector2D ToWidgetPos = MouseWidgetPos - DragDrop->DeltaWidgetPos;
	FIntPoint ToItemSlotPos = FIntPoint(ToWidgetPos.X / Item::UnitInventorySlotSize.X, ToWidgetPos.Y / Item::UnitInventorySlotSize.Y);

	if (PrevDragOverSlotPos == ToItemSlotPos)
		return true;
	
	PrevDragOverSlotPos = ToItemSlotPos;
	
	UD1ItemEntryWidget* FromEntryWidget = DragDrop->FromEntryWidget;
	UD1ItemInstance* FromItemInstance = FromEntryWidget->GetItemInstance();
	check(FromItemInstance);
	
	const UD1ItemTemplate& FromItemTemplate = UD1ItemData::Get().FindItemTemplateByID(FromItemInstance->GetItemTemplateID());
	const FIntPoint& FromItemSlotCount = FromItemTemplate.SlotCount;

	int32 MovableCount = 0;
	if (UD1InventoryManagerComponent* FromInventoryManager = DragDrop->FromInventoryManager)
	{
		MovableCount = InventoryManager->CanMoveOrMergeItem(FromInventoryManager, DragDrop->FromItemSlotPos, ToItemSlotPos);
	}
	else if (UD1EquipmentManagerComponent* FromEquipmentManager = DragDrop->FromEquipmentManager)
	{
		MovableCount = InventoryManager->CanMoveOrMergeItem(FromEquipmentManager, DragDrop->FromEquipmentSlotType, ToItemSlotPos);
	}
	
	UnhoverSlots();

	const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();
	
	const FIntPoint StartSlotPos = FIntPoint(FMath::Max(0, ToItemSlotPos.X), FMath::Max(0, ToItemSlotPos.Y));
	const FIntPoint EndSlotPos   = FIntPoint(FMath::Min(ToItemSlotPos.X + FromItemSlotCount.X, InventorySlotCount.X),
											 FMath::Min(ToItemSlotPos.Y + FromItemSlotCount.Y, InventorySlotCount.Y));
	
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

	DragDrop->ToInventoryManager = InventoryManager;
    DragDrop->ToItemSlotPos = ToItemSlotPos;

	UD1ItemManagerComponent* ItemManager = GetOwningPlayer()->FindComponentByClass<UD1ItemManagerComponent>();
	check(ItemManager);
	
	if (UD1InventoryManagerComponent* FromInventoryManager = DragDrop->FromInventoryManager)
	{
		ItemManager->Server_InventoryToInventory(FromInventoryManager, DragDrop->FromItemSlotPos, InventoryManager, ToItemSlotPos);
	}
	else if (UD1EquipmentManagerComponent* FromEquipmentManager = DragDrop->FromEquipmentManager)
	{
		ItemManager->Server_EquipmentToInventory(FromEquipmentManager, DragDrop->FromEquipmentSlotType, InventoryManager, ToItemSlotPos);
	}
	return true;
}

void UD1InventorySlotsWidget::ConstructUI(FGameplayTag Channel, const FInventoryInitializeMessage& Message)
{
	if (Message.InventoryManager == nullptr)
		return;
	
	InventoryManager = Message.InventoryManager;
	
	const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();
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
	
	const TArray<FD1InventoryEntry>& Entries = InventoryManager->GetAllEntries();
	for (int32 i = 0; i < Entries.Num(); i++)
	{
		const FD1InventoryEntry& Entry = Entries[i];
		if (UD1ItemInstance* ItemInstance = Entry.GetItemInstance())
		{
			FIntPoint ItemSlotPos = FIntPoint(i % InventorySlotCount.X, i / InventorySlotCount.X);
			OnInventoryEntryChanged(ItemSlotPos, ItemInstance, Entry.GetItemCount());
		}
	}
	DelegateHandle = InventoryManager->OnInventoryEntryChanged.AddUObject(this, &ThisClass::OnInventoryEntryChanged);
}

void UD1InventorySlotsWidget::DestructUI()
{
	InventoryManager->OnInventoryEntryChanged.Remove(DelegateHandle);
	DelegateHandle.Reset();

	CanvasPanel_Entries->ClearChildren();
	EntryWidgets.Reset();
	
	GridPanel_Slots->ClearChildren();
	SlotWidgets.Reset();
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

void UD1InventorySlotsWidget::OnInventoryEntryChanged(const FIntPoint& InItemSlotPos, UD1ItemInstance* InItemInstance, int32 InItemCount)
{
	const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();
	int32 SlotIndex = InItemSlotPos.Y * InventorySlotCount.X + InItemSlotPos.X;

	if (UD1InventoryEntryWidget* EntryWidget = EntryWidgets[SlotIndex])
	{
		if (UD1ItemInstance* ItemInstance = EntryWidget->GetItemInstance())
		{
			if (ItemInstance == InItemInstance)
			{
				EntryWidget->RefreshItemCount(InItemCount);
				return;
			}
			else
			{
				const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());

				const FIntPoint StartSlotPos = InItemSlotPos;
				const FIntPoint EndSlotPos = InItemSlotPos + ItemTemplate.SlotCount;
				
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
		}
		
		CanvasPanel_Entries->RemoveChild(EntryWidget);
		EntryWidgets[SlotIndex] = nullptr;
	}
	
	if (InItemInstance)
	{
		const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(InItemInstance->GetItemTemplateID());
		
		UD1InventoryEntryWidget* EntryWidget = CreateWidget<UD1InventoryEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
		EntryWidgets[SlotIndex] = EntryWidget;

		UCanvasPanelSlot* CanvasPanelSlot = CanvasPanel_Entries->AddChildToCanvas(EntryWidget);
		CanvasPanelSlot->SetAutoSize(true);
		CanvasPanelSlot->SetPosition(FVector2D(InItemSlotPos.X * Item::UnitInventorySlotSize.X, InItemSlotPos.Y * Item::UnitInventorySlotSize.Y));
		
		EntryWidget->Init(this, InItemInstance, InItemCount);
		
		const FIntPoint StartSlotPos = InItemSlotPos;
		const FIntPoint EndSlotPos = InItemSlotPos + ItemTemplate.SlotCount;
		
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
