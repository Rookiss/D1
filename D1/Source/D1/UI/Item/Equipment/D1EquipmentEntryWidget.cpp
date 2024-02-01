#include "D1EquipmentEntryWidget.h"

#include "D1EquipmentSlotsWidget.h"
#include "D1EquipmentSlotWidget.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemInstance.h"
#include "System/D1AssetManager.h"
#include "UI/D1HUD.h"
#include "UI/D1SceneWidget.h"
#include "UI/Item/Drag/D1ItemDragDrop.h"
#include "UI/Item/Drag/D1ItemDragWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1EquipmentEntryWidget)

UD1EquipmentEntryWidget::UD1EquipmentEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1EquipmentEntryWidget::Init(UD1EquipmentManagerComponent* InEquipmentManagerComponent, const FVector2D& WidgetSize, UD1ItemInstance* InItemInstance, EEquipmentSlotType InEquipmentSlotType)
{
	EquipmentManagerComponent = InEquipmentManagerComponent;
	ItemInstance = InItemInstance;
	
	SizeBox_Root->SetWidthOverride(WidgetSize.X);
	SizeBox_Root->SetHeightOverride(WidgetSize.Y);

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemInstance->GetItemID());
	Image_Icon->SetBrushFromTexture(ItemDef.IconTexture);

	ItemInstance = InItemInstance;
	EquipmentSlotType = InEquipmentSlotType;
	
	EntryWidgetOffset = (WidgetSize - ItemDef.ItemSlotCount * UD1InventorySlotWidget::UnitSlotSize) / 2.f / WidgetSize;
}

void UD1EquipmentEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	DragWidgetClass = UD1AssetManager::GetSubclassByName<UD1ItemDragWidget>("DragWidget");

	Image_Hover->SetRenderOpacity(0.f);
}

void UD1EquipmentEntryWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

void UD1EquipmentEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
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

FReply UD1EquipmentEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	CachedDeltaWidgetPos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		Reply.DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	return Reply;
}

void UD1EquipmentEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	RefreshWidgetOpacity(false);

	const UD1ItemData* ItemData = UD1AssetManager::GetItemData();
	const FD1ItemDefinition& ItemDef = ItemData->GetItemDefByID(ItemInstance->GetItemID());
	
	UD1ItemDragWidget* DragWidget = CreateWidget<UD1ItemDragWidget>(GetOwningPlayer(), DragWidgetClass);
	FVector2D EntityWidgetSize = FVector2D(ItemDef.ItemSlotCount * UD1InventorySlotWidget::UnitSlotSize);
	DragWidget->Init(EntityWidgetSize, ItemDef.IconTexture, 0);
	
	UD1ItemDragDrop* DragDrop = NewObject<UD1ItemDragDrop>();
	DragDrop->DefaultDragVisual = DragWidget;
	DragDrop->Pivot = EDragPivot::CenterCenter;
	DragDrop->EquipmentEntryWidget = this;
	DragDrop->FromEquipmentManager = EquipmentManagerComponent;
	DragDrop->FromEquipmentSlotType = EquipmentSlotType;
	DragDrop->DeltaWidgetPos = (EntityWidgetSize / 2.f) - (UD1InventorySlotWidget::UnitSlotSize / 2.f);
	OutOperation = DragDrop;
}

void UD1EquipmentEntryWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	if (UD1ItemDragDrop* DragDrop = Cast<UD1ItemDragDrop>(InOperation))
	{
		if (UD1EquipmentEntryWidget* EntryWidget = DragDrop->EquipmentEntryWidget)
		{
			EntryWidget->RefreshWidgetOpacity(true);
		}
	}
}

void UD1EquipmentEntryWidget::RefreshWidgetOpacity(bool bClearlyVisible)
{
	SetRenderOpacity(bClearlyVisible ? 1.f : 0.5f);
}
