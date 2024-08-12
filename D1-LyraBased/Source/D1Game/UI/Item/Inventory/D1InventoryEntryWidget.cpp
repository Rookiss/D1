#include "D1InventoryEntryWidget.h"

#include "D1InventorySlotsWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Equippable.h"
#include "Item/Managers/D1InventoryManagerComponent.h"
#include "Item/Managers/D1ItemManagerComponent.h"
#include "UI/Item/D1ItemDragDrop.h"
#include "UI/Item/D1ItemDragWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventoryEntryWidget)

UD1InventoryEntryWidget::UD1InventoryEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1InventoryEntryWidget::Init(UD1InventorySlotsWidget* InSlotsWidget, UD1ItemInstance* InItemInstance, int32 InItemCount)
{
	SlotsWidget = InSlotsWidget;
	RefreshUI(InItemInstance, InItemCount);
	
	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());

	FVector2D WidgetSize = FVector2D(ItemTemplate.SlotCount.X * Item::UnitInventorySlotSize.X, ItemTemplate.SlotCount.Y * Item::UnitInventorySlotSize.Y);
	SizeBox_Root->SetWidthOverride(WidgetSize.X);
	SizeBox_Root->SetHeightOverride(WidgetSize.Y);
}

FReply UD1InventoryEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
	FVector2D MouseWidgetPos = SlotsWidget->GetCachedGeometry().AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	FVector2D ItemWidgetPos = SlotsWidget->GetCachedGeometry().AbsoluteToLocal(InGeometry.LocalToAbsolute(Item::UnitInventorySlotSize / 2.f));
	FIntPoint ItemSlotPos = FIntPoint(ItemWidgetPos.X / Item::UnitInventorySlotSize.X, ItemWidgetPos.Y / Item::UnitInventorySlotSize.Y);
	
	CachedFromSlotPos = ItemSlotPos;
	CachedDeltaWidgetPos = MouseWidgetPos - ItemWidgetPos;
	
	if (Reply.IsEventHandled() == false && UWidgetBlueprintLibrary::IsDragDropping() == false && InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		UD1ItemManagerComponent* ItemManager = GetOwningPlayer()->FindComponentByClass<UD1ItemManagerComponent>();
		UD1InventoryManagerComponent* FromInventoryManager = SlotsWidget->GetInventoryManagerComponent();

		if (ItemManager && FromInventoryManager)
		{
			ItemManager->Server_QuickFromInventory(FromInventoryManager, ItemSlotPos);
			return FReply::Handled();
		}
	}
	
	return Reply;
}

void UD1InventoryEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	
	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
	
	UD1ItemDragWidget* DragWidget = CreateWidget<UD1ItemDragWidget>(GetOwningPlayer(), DragWidgetClass);
	FVector2D DragWidgetSize = FVector2D(ItemTemplate.SlotCount.X * Item::UnitInventorySlotSize.X, ItemTemplate.SlotCount.Y * Item::UnitInventorySlotSize.Y);
	DragWidget->Init(DragWidgetSize, ItemTemplate.IconTexture, ItemCount);
	
	UD1ItemDragDrop* DragDrop = NewObject<UD1ItemDragDrop>();
	DragDrop->DefaultDragVisual = DragWidget;
	DragDrop->Pivot = EDragPivot::TopLeft;
	DragDrop->Offset = -((CachedDeltaWidgetPos + Item::UnitInventorySlotSize / 2.f) / DragWidgetSize);
	DragDrop->FromEntryWidget = this;
	DragDrop->FromInventoryManager = SlotsWidget->GetInventoryManagerComponent();
	DragDrop->FromItemSlotPos = CachedFromSlotPos;
	DragDrop->FromItemInstance = ItemInstance;
	DragDrop->DeltaWidgetPos = CachedDeltaWidgetPos;
	OutOperation = DragDrop;
}
