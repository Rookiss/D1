#include "D1InventoryEntryWidget.h"

#include "D1InventorySlotsWidget.h"
#include "D1InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemInstance.h"
#include "Item/Fragments/D1ItemFragment_Stackable.h"
#include "System/D1AssetManager.h"
#include "UI/D1HUD.h"
#include "UI/D1SceneWidget.h"
#include "UI/Item/Drag/D1ItemDragDrop.h"
#include "UI/Item/Drag/D1ItemDragWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventoryEntryWidget)

UD1InventoryEntryWidget::UD1InventoryEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
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

void UD1InventoryEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	DragWidgetClass = UD1AssetManager::GetSubclassByName<UD1ItemDragWidget>("DragWidget");
	
	Image_Hover->SetRenderOpacity(0.f);
	Text_Count->SetText(FText::GetEmpty());
}

void UD1InventoryEntryWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	Image_Hover->SetRenderOpacity(1.f);

	if (AD1HUD* HUD = Cast<AD1HUD>(GetOwningPlayer()->GetHUD()))
	{
		if (UD1SceneWidget* SceneWidget = HUD->SceneWidget)
		{
			SceneWidget->ShowItemHoverWidget(ItemInstance);
		}
	}
}

void UD1InventoryEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	Image_Hover->SetRenderOpacity(0.f);

	if (AD1HUD* HUD = Cast<AD1HUD>(GetOwningPlayer()->GetHUD()))
	{
		if (UD1SceneWidget* SceneWidget = HUD->SceneWidget)
		{
			SceneWidget->HideItemHoverWidget();
		}
	}
}

FReply UD1InventoryEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
	const FVector2D& UnitSlotSize = UD1InventorySlotWidget::UnitSlotSize;
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
	
	UD1ItemDragWidget* DragWidget = CreateWidget<UD1ItemDragWidget>(GetOwningPlayer(), DragWidgetClass);
	const FVector2D& UnitSlotSize = UD1InventorySlotWidget::UnitSlotSize;
	FVector2D EntityWidgetSize = FVector2D(ItemDef.ItemSlotCount.X * UnitSlotSize.X, ItemDef.ItemSlotCount.Y * UnitSlotSize.Y);
	DragWidget->Init(EntityWidgetSize, ItemDef.IconTexture, ItemCount);
	
	UD1ItemDragDrop* DragDrop = NewObject<UD1ItemDragDrop>();
	DragDrop->DefaultDragVisual = DragWidget;
	DragDrop->Pivot = EDragPivot::MouseDown;
	DragDrop->InventoryEntryWidget = this;
	DragDrop->FromInventoryManager = SlotsWidget->GetInventoryManagerComponent();
	DragDrop->FromItemSlotPos = CachedFromSlotPos;
	DragDrop->DeltaWidgetPos = CachedDeltaWidgetPos;
	OutOperation = DragDrop;
}

void UD1InventoryEntryWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	
	if (UD1ItemDragDrop* DragDrop = Cast<UD1ItemDragDrop>(InOperation))
	{
		if (UD1InventoryEntryWidget* EntryWidget = DragDrop->InventoryEntryWidget)
		{
			EntryWidget->RefreshWidgetOpacity(true);
		}
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
