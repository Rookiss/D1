#include "D1InventoryEntryWidget.h"

#include "D1InventoryDragDrop.h"
#include "D1InventoryDragWidget.h"
#include "D1InventorySlotsWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
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
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	return Reply.NativeReply;
}

void UD1InventoryEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	
	if (ItemInstance == nullptr)
		return;
	
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	check(ItemData);
	
	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemInstance->GetItemID());
	
	UD1InventoryDragWidget* DragWidget = CreateWidget<UD1InventoryDragWidget>(GetOwningPlayer(), DragWidgetClass);
	const FVector2D& UnitSlotSize = SlotsWidget->GetUnitSlotSize();
	FVector2D WidgetSize = FVector2D(ItemDef.ItemSlotCount.X * UnitSlotSize.X, ItemDef.ItemSlotCount.Y * UnitSlotSize.Y);
	DragWidget->Init(WidgetSize, ItemDef.IconTexture, ItemCount);
	
	UD1InventoryDragDrop* DragDrop = NewObject<UD1InventoryDragDrop>();
	DragDrop->DefaultDragVisual = DragWidget;
	DragDrop->Pivot = EDragPivot::MouseDown;
	OutOperation = DragDrop;
}

void UD1InventoryEntryWidget::Init(UD1InventorySlotsWidget* InSlotsWidget, const FVector2D& NewWidgetSize, UD1ItemInstance* NewItemInstance, int32 NewItemCount)
{
	SlotsWidget = InSlotsWidget;
	
	SizeBox_Root->SetWidthOverride(NewWidgetSize.X);
	SizeBox_Root->SetHeightOverride(NewWidgetSize.Y);
	
	ItemInstance = NewItemInstance;
	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	check(ItemData);

	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemInstance->GetItemID());
	Image_Icon->SetBrushFromTexture(ItemDef.IconTexture);

	if (ItemDef.FindFragmentByClass<UD1ItemFragment_Stackable>())
	{
		RefreshItemCount(NewItemCount);
	}
}

void UD1InventoryEntryWidget::RefreshItemCount(int32 NewItemCount)
{
	ItemCount = NewItemCount;
	Text_Count->SetText((ItemCount >= 2) ? FText::AsNumber(ItemCount) : FText::GetEmpty());
}
