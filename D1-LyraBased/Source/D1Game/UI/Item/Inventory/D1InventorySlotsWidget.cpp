#include "D1InventorySlotsWidget.h"

#include "AbilitySystemComponent.h"
#include "D1InventoryEntryWidget.h"
#include "D1InventorySlotWidget.h"
#include "D1InventoryValidWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Data/D1ItemData.h"
#include "Data/D1UIData.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Item/D1ItemInstance.h"
#include "Item/D1ItemTemplate.h"
#include "Item/Managers/D1EquipmentManagerComponent.h"
#include "Item/Managers/D1InventoryManagerComponent.h"
#include "Item/Managers/D1ItemManagerComponent.h"
#include "UI/Item/D1ItemDragDrop.h"
#include "UI/Item/Equipment/D1EquipmentEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventorySlotsWidget)

UD1InventorySlotsWidget::UD1InventorySlotsWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TitleText = FText::FromString(TEXT("Title"));
}

void UD1InventorySlotsWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	Text_Title->SetText(TitleText);
}

void UD1InventorySlotsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageListenerHandle = MessageSubsystem.RegisterListener(MessageChannelTag, this, &ThisClass::ConstructUI);
}

void UD1InventorySlotsWidget::NativeDestruct()
{
	DestructUI();
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(MessageListenerHandle);
	
	Super::NativeDestruct();
}

bool UD1InventorySlotsWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
	
	UD1ItemDragDrop* DragDrop = Cast<UD1ItemDragDrop>(InOperation);
	if (DragDrop == nullptr)
		return false;

	FIntPoint UnitInventorySlotSize = UD1UIData::Get().UnitInventorySlotSize;
	
	FVector2D MouseWidgetPos = GetSlotContainerGeometry().AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
	FVector2D ToWidgetPos = MouseWidgetPos - DragDrop->DeltaWidgetPos;
	FIntPoint ToItemSlotPos = FIntPoint(ToWidgetPos.X / UnitInventorySlotSize.X, ToWidgetPos.Y / UnitInventorySlotSize.Y);

	if (PrevDragOverSlotPos == ToItemSlotPos)
		return true;
	
	PrevDragOverSlotPos = ToItemSlotPos;
	
	UD1ItemEntryWidget* FromEntryWidget = DragDrop->FromEntryWidget;
	UD1ItemInstance* FromItemInstance = FromEntryWidget->GetItemInstance();
	if (FromItemInstance == nullptr)
		return false;
	
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
	
	ResetValidSlots();

	const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();
	
	const FIntPoint StartSlotPos = FIntPoint(FMath::Max(0, ToItemSlotPos.X), FMath::Max(0, ToItemSlotPos.Y));
	const FIntPoint EndSlotPos   = FIntPoint(FMath::Min(ToItemSlotPos.X + FromItemSlotCount.X, InventorySlotCount.X),
											 FMath::Min(ToItemSlotPos.Y + FromItemSlotCount.Y, InventorySlotCount.Y));

	ESlotState SlotState = (MovableCount > 0) ? ESlotState::Valid : ESlotState::Invalid;
	for (int32 y = StartSlotPos.Y; y < EndSlotPos.Y; y++)
	{
		for (int32 x = StartSlotPos.X; x < EndSlotPos.X; x++)
		{
			int32 Index = y * InventorySlotCount.X + x;
			if (UD1InventoryValidWidget* ValidWidget = ValidWidgets[Index])
			{
				ValidWidget->ChangeSlotState(SlotState);
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

	FIntPoint UnitInventorySlotSize = UD1UIData::Get().UnitInventorySlotSize;
	
	UD1ItemDragDrop* DragDrop = Cast<UD1ItemDragDrop>(InOperation);
	check(DragDrop);

	UD1ItemEntryWidget* FromEntryWidget = DragDrop->FromEntryWidget;
	FromEntryWidget->RefreshWidgetOpacity(true);
	
	FVector2D MouseWidgetPos = GetSlotContainerGeometry().AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
	FVector2D ToWidgetPos = MouseWidgetPos - DragDrop->DeltaWidgetPos;
	FIntPoint ToItemSlotPos = FIntPoint(ToWidgetPos.X / UnitInventorySlotSize.X, ToWidgetPos.Y / UnitInventorySlotSize.Y);

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
	const int32 InventorySlotNum = InventorySlotCount.X * InventorySlotCount.Y;
	SlotWidgets.SetNum(InventorySlotNum);
	ValidWidgets.SetNum(InventorySlotNum);
	EntryWidgets.SetNum(InventorySlotNum);
	
	for (int32 y = 0; y < InventorySlotCount.Y; y++)
	{
		for (int32 x = 0; x < InventorySlotCount.X; x++)
		{
			TSubclassOf<UD1InventorySlotWidget> SlotWidgetClass = UD1UIData::Get().InventorySlotWidgetClass;
			UD1InventorySlotWidget* SlotWidget = CreateWidget<UD1InventorySlotWidget>(GetOwningPlayer(), SlotWidgetClass);
			SlotWidgets[y * InventorySlotCount.X + x] = SlotWidget;
			GridPanel_Slots->AddChildToUniformGrid(SlotWidget, y, x);

			TSubclassOf<UD1InventoryValidWidget> ValidWidgetClass = UD1UIData::Get().InventoryValidWidgetClass;
			UD1InventoryValidWidget* ValidWidget = CreateWidget<UD1InventoryValidWidget>(GetOwningPlayer(), ValidWidgetClass);
			ValidWidgets[y * InventorySlotCount.X + x] = ValidWidget;
			GridPanel_ValidSlots->AddChildToUniformGrid(ValidWidget, y, x);
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
	EntryChangedDelegateHandle = InventoryManager->OnInventoryEntryChanged.AddUObject(this, &ThisClass::OnInventoryEntryChanged);
}

void UD1InventorySlotsWidget::DestructUI()
{
	InventoryManager->OnInventoryEntryChanged.Remove(EntryChangedDelegateHandle);
	EntryChangedDelegateHandle.Reset();

	CanvasPanel_Entries->ClearChildren();
	EntryWidgets.Reset();
	
	GridPanel_Slots->ClearChildren();
	SlotWidgets.Reset();
}

void UD1InventorySlotsWidget::ResetValidSlots()
{
	for (UD1InventoryValidWidget* ValidWidget : ValidWidgets)
	{
		if (ValidWidget)
		{
			ValidWidget->ChangeSlotState(ESlotState::Default);
		}
	}
}

void UD1InventorySlotsWidget::FinishDrag()
{
	ResetValidSlots();
	PrevDragOverSlotPos = FIntPoint(-1, -1);
}

void UD1InventorySlotsWidget::OnInventoryEntryChanged(const FIntPoint& InItemSlotPos, UD1ItemInstance* InItemInstance, int32 InItemCount)
{
	const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();
	int32 SlotIndex = InItemSlotPos.Y * InventorySlotCount.X + InItemSlotPos.X;

	if (UD1InventoryEntryWidget* EntryWidget = EntryWidgets[SlotIndex])
	{
		UD1ItemInstance* ItemInstance = EntryWidget->GetItemInstance();
		if (ItemInstance && ItemInstance == InItemInstance)
		{
			EntryWidget->RefreshItemCount(InItemCount);
			return;
		}
		
		CanvasPanel_Entries->RemoveChild(EntryWidget);
		EntryWidgets[SlotIndex] = nullptr;
	}
	
	if (InItemInstance)
	{
		FIntPoint UnitInventorySlotSize = UD1UIData::Get().UnitInventorySlotSize;
		
		TSubclassOf<UD1InventoryEntryWidget> EntryWidgetClass = UD1UIData::Get().InventoryEntryWidgetClass;
		UD1InventoryEntryWidget* EntryWidget = CreateWidget<UD1InventoryEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
		EntryWidgets[SlotIndex] = EntryWidget;
		
		EntryWidget->Init(this, InItemInstance, InItemCount);
		
		UCanvasPanelSlot* CanvasPanelSlot = CanvasPanel_Entries->AddChildToCanvas(EntryWidget);
		CanvasPanelSlot->SetAutoSize(true);
		CanvasPanelSlot->SetPosition(FVector2D(InItemSlotPos.X * UnitInventorySlotSize.X, InItemSlotPos.Y * UnitInventorySlotSize.Y));
	}
}

const FGeometry& UD1InventorySlotsWidget::GetSlotContainerGeometry() const
{
	return Overlay_Slots->GetCachedGeometry();
}
