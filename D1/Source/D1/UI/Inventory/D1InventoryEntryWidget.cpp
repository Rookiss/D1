#include "D1InventoryEntryWidget.h"

#include "D1InventoryDragDrop.h"
#include "D1InventoryDragWidget.h"
#include "D1InventorySlotsWidget.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Data/D1ItemData.h"
#include "Inventory/D1ItemInstance.h"
#include "Inventory/Fragments/D1ItemFragment_Stackable.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventoryEntryWidget)

UD1InventoryEntryWidget::UD1InventoryEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1InventoryEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	DragWidgetClass = UD1AssetManager::GetSubclassByName<UD1InventoryDragWidget>("DragWidget");
	
	Image_Hover->SetRenderOpacity(0.f);
	Text_Count->SetText(FText::GetEmpty());
}

void UD1InventoryEntryWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	Image_Hover->SetRenderOpacity(1.f);
}

void UD1InventoryEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	Image_Hover->SetRenderOpacity(0.f);
}

FReply UD1InventoryEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
	const FVector2D& UnitSlotSize = SlotsWidget->GetUnitSlotSize();
	FVector2D MouseWidgetPos = SlotsWidget->GetCachedGeometry().AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	FVector2D ItemWidgetPos = SlotsWidget->GetCachedGeometry().AbsoluteToLocal(InGeometry.LocalToAbsolute(UnitSlotSize / 2.f));
	FIntPoint ItemSlotPos = FIntPoint(ItemWidgetPos.X / UnitSlotSize.X, ItemWidgetPos.Y / UnitSlotSize.Y);
	
	CachedFromSlotPos = ItemSlotPos;
	CachedDeltaWidgetPos = MouseWidgetPos - ItemWidgetPos;

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		Reply.DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	return Reply;
}

void UD1InventoryEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	RefreshWidgetOpacity(false);
	
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemInstance->GetItemID());
	
	UD1InventoryDragWidget* DragWidget = CreateWidget<UD1InventoryDragWidget>(GetOwningPlayer(), DragWidgetClass);
	const FVector2D& UnitSlotSize = SlotsWidget->GetUnitSlotSize();
	FVector2D EntityWidgetSize = FVector2D(ItemDef.ItemSlotCount.X * UnitSlotSize.X, ItemDef.ItemSlotCount.Y * UnitSlotSize.Y);
	DragWidget->Init(EntityWidgetSize, ItemDef.IconTexture, ItemCount);
	
	UD1InventoryDragDrop* DragDrop = NewObject<UD1InventoryDragDrop>();
	DragDrop->DefaultDragVisual = DragWidget;
	DragDrop->Pivot = EDragPivot::MouseDown;
	DragDrop->EntryWidget = this;
	DragDrop->FromSlotPos = CachedFromSlotPos;
	DragDrop->DeltaWidgetPos = CachedDeltaWidgetPos;
	OutOperation = DragDrop;
}

void UD1InventoryEntryWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	
	if (UD1InventoryDragDrop* DragDrop = Cast<UD1InventoryDragDrop>(InOperation))
	{
		if (UD1InventoryEntryWidget* EntryWidget = DragDrop->EntryWidget)
		{
			EntryWidget->RefreshWidgetOpacity(true);
		}
	}
}

void UD1InventoryEntryWidget::Init(UD1InventorySlotsWidget* InSlotsWidget, const FVector2D& InWidgetSize, UD1ItemInstance* InItemInstance, int32 InItemCount)
{
	SlotsWidget = InSlotsWidget;
	
	SizeBox_Root->SetWidthOverride(InWidgetSize.X);
	SizeBox_Root->SetHeightOverride(InWidgetSize.Y);
	
	ItemInstance = InItemInstance;
	
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemInstance->GetItemID());
	Image_Icon->SetBrushFromTexture(ItemDef.IconTexture);

	if (ItemDef.FindFragmentByClass<UD1ItemFragment_Stackable>())
	{
		RefreshItemCount(InItemCount);
	}
}

void UD1InventoryEntryWidget::RefreshItemCount(int32 NewItemCount)
{
	ItemCount = NewItemCount;
	Text_Count->SetText((ItemCount >= 2) ? FText::AsNumber(ItemCount) : FText::GetEmpty());
}

void UD1InventoryEntryWidget::RefreshWidgetOpacity(bool bClearlyVisible)
{
	SetRenderOpacity(bClearlyVisible ? 1.f : 0.5f);
}
