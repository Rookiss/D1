#include "D1ItemEntryWidget.h"

#include "Components/Image.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemInstance.h"
#include "Item/D1ItemTemplate.h"
#include "System/LyraAssetManager.h"
#include "D1ItemDragWidget.h"
#include "D1ItemHoversWidget.h"
#include "Blueprint/SlateBlueprintLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemEntryWidget)

UD1ItemEntryWidget::UD1ItemEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ItemEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	DragWidgetClass = ULyraAssetManager::GetSubclassByName<UD1ItemDragWidget>("DragWidgetClass");
	HoversWidgetClass = ULyraAssetManager::GetSubclassByName<UD1ItemHoversWidget>("HoversWidgetClass");
}

void UD1ItemEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Image_Hover->SetRenderOpacity(0.f);
}

void UD1ItemEntryWidget::NativeDestruct()
{
	if (HoversWidget)
	{
		HoversWidget->RemoveFromParent();
		HoversWidget = nullptr;
	}
	
	Super::NativeDestruct();
}

void UD1ItemEntryWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	Image_Hover->SetRenderOpacity(1.f);

	if (HoversWidget == nullptr)
	{
		HoversWidget = CreateWidget<UD1ItemHoversWidget>(GetOwningPlayer(), HoversWidgetClass);
	}
	HoversWidget->RefreshUI(ItemInstance);
	HoversWidget->AddToViewport();
}

FReply UD1ItemEntryWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (HoversWidget)
	{
		HoversWidget->SetPosition(InMouseEvent.GetScreenSpacePosition());
		return FReply::Handled();
	}
	
	return Reply;
}

void UD1ItemEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	Image_Hover->SetRenderOpacity(0.f);

	if (HoversWidget)
	{
		HoversWidget->RemoveFromParent();
		HoversWidget = nullptr;
	}
}

FReply UD1ItemEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		Reply.DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	return Reply;
}

void UD1ItemEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	RefreshWidgetOpacity(false);
}

void UD1ItemEntryWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	RefreshWidgetOpacity(true);
}

void UD1ItemEntryWidget::RefreshWidgetOpacity(bool bClearlyVisible)
{
	SetRenderOpacity(bClearlyVisible ? 1.f : 0.5f);
}

void UD1ItemEntryWidget::SetItemInstance(UD1ItemInstance* NewItemInstance)
{
	ItemInstance = NewItemInstance;
	check(ItemInstance);

	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
	Image_Icon->SetBrushFromTexture(ItemTemplate.IconTexture, true);
}
