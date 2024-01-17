#include "D1InventoryEntryWidget.h"

#include "D1InventoryDragDrop.h"
#include "D1InventorySlotWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Data/D1ItemData.h"
#include "Inventory/D1ItemInstance.h"
#include "System/D1AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventoryEntryWidget)

UD1InventoryEntryWidget::UD1InventoryEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1InventoryEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

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
	// TODO
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	// FVector2D MousePosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	// FIntPoint SlotPosition = FIntPoint(MousePosition.X / SlotSize.X, MousePosition.Y / SlotSize.Y);
	//
	// int32 SlotIndex = SlotPosition.Y * InventorySlotCount.X + SlotPosition.X;
	// if (SlotWidgets.IsValidIndex(SlotIndex))
	// {
	// 	if (UD1InventorySlotWidget* SlotWidget = SlotWidgets[SlotIndex])
	// 	{
	// 		if (UD1InventoryEntryWidget* EntryWidget = SlotWidget->GetEntryWidget())
	// 		{
	// 			DragCachedWidget = EntryWidget;
	// 			FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	// 			return Reply.NativeReply;
	// 		}
	// 	}
	// }
	return FReply::Handled();
}

void UD1InventoryEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	// TODO
	if (ItemInstance == nullptr)
		return;
	
	// const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	// check(ItemData);
	// 	
	// const UD1ItemInstance* ItemInstance = DragCachedWidget->ItemInstance;
	// const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemInstance->GetItemID());
	//
	// UD1InventoryEntryWidget* DragWidget = CreateWidget<UD1InventoryEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
	// DragWidget->Init(FVector2D(ItemDef.ItemSlotCount.X * SlotSize.X, ItemDef.ItemSlotCount.Y * SlotSize.Y));
	// DragWidget->RefreshUI(DragCachedWidget->ItemInstance, DragCachedWidget->ItemCount);
	//
	// UD1InventoryDragDrop* DragDrop = NewObject<UD1InventoryDragDrop>();
	// DragDrop->DefaultDragVisual = DragWidget;
	// DragDrop->Pivot = EDragPivot::MouseDown;
	// OutOperation = DragDrop;
}

void UD1InventoryEntryWidget::Init(UD1InventorySlotsWidget* InSlotsWidget, const FVector2D& NewSize, UD1ItemInstance* NewItemInstance)
{
	SlotsWidget = InSlotsWidget;
	
	SizeBox_Root->SetWidthOverride(NewSize.X);
	SizeBox_Root->SetHeightOverride(NewSize.Y);
	
	ItemInstance = NewItemInstance;
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	check(ItemData);

	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemInstance->GetItemID());
	Image_Icon->SetBrushFromTexture(ItemDef.IconTexture);
}

void UD1InventoryEntryWidget::RefreshItemCount(int32 NewItemCount)
{
	ItemCount = NewItemCount;
	FText CountText = (ItemCount == 1) ? FText::GetEmpty() : FText::AsNumber(ItemCount);
	Text_Count->SetText(CountText);
}
