#include "D1ItemEntryWidget.h"

#include "Components/Image.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemInstance.h"
#include "Item/D1ItemTemplate.h"
#include "System/LyraAssetManager.h"
#include "D1ItemDragWidget.h"
#include "D1ItemHoverWidget.h"
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
	HoverWidgetClass = ULyraAssetManager::GetSubclassByName<UD1ItemHoverWidget>("HoverWidgetClass");
}

void UD1ItemEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Image_Hover->SetRenderOpacity(0.f);
}

void UD1ItemEntryWidget::NativeDestruct()
{
	if (HoverWidget)
	{
		HoverWidget->RemoveFromParent();
		HoverWidget = nullptr;
	}
	
	Super::NativeDestruct();
}

void UD1ItemEntryWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	Image_Hover->SetRenderOpacity(1.f);

	if (HoverWidget == nullptr)
	{
		HoverWidget = CreateWidget<UD1ItemHoverWidget>(GetOwningPlayer(), HoverWidgetClass);
	}
	HoverWidget->RefreshUI(ItemInstance);
	HoverWidget->AddToViewport();
}

FReply UD1ItemEntryWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (HoverWidget)
	{
		FVector2D Margin = FVector2D(5.f, 5.f);
		FVector2D AbsoluteMousePos = InMouseEvent.GetScreenSpacePosition();
		FVector2D HoverWidgetSize = HoverWidget->GetCachedGeometry().GetAbsoluteSize();
		FVector2D HoverWidgetStartPos = AbsoluteMousePos + Margin;
		FVector2D HoverWidgetEndPos = HoverWidgetStartPos + HoverWidgetSize;

		FVector2D ViewportSize;
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(ViewportSize);
		}
		
		if (HoverWidgetEndPos.X > ViewportSize.X)
		{
			HoverWidgetStartPos.X = AbsoluteMousePos.X - (Margin.X + HoverWidgetSize.X);
		}

		if (HoverWidgetEndPos.Y > ViewportSize.Y)
		{
			HoverWidgetStartPos.Y = AbsoluteMousePos.Y - (Margin.Y + HoverWidgetSize.Y);
		}
		
		FVector2D PixelPos, ViewportPos;
		USlateBlueprintLibrary::AbsoluteToViewport(this, HoverWidgetStartPos, PixelPos, ViewportPos);
		HoverWidget->SetPositionInViewport({HoverWidgetSize.X, HoverWidgetSize.Y});
		
		return FReply::Handled();
	}
	
	return Reply;
}

void UD1ItemEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	Image_Hover->SetRenderOpacity(0.f);

	if (HoverWidget)
	{
		HoverWidget->RemoveFromParent();
		HoverWidget = nullptr;
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
