#include "D1ItemEntryWidget.h"

#include "Components/Image.h"
#include "Data/D1ItemData.h"
#include "Item/D1ItemInstance.h"
#include "Item/D1ItemTemplate.h"
#include "D1ItemDragWidget.h"
#include "D1ItemHoverWidget.h"
#include "Components/TextBlock.h"
#include "Data/D1UIData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ItemEntryWidget)

UD1ItemEntryWidget::UD1ItemEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1ItemEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Text_Count->SetText(FText::GetEmpty());
}

void UD1ItemEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Image_Hover->SetVisibility(ESlateVisibility::Hidden);
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

	Image_Hover->SetVisibility(ESlateVisibility::Visible);

	if (HoverWidget == nullptr)
	{
		TSubclassOf<UD1ItemHoverWidget> HoverWidgetClass = UD1UIData::Get().ItemHoverWidgetClass;
		HoverWidget = CreateWidget<UD1ItemHoverWidget>(GetOwningPlayer(), HoverWidgetClass);
	}

	if (HoverWidget)
	{
		HoverWidget->RefreshUI(ItemInstance);
		HoverWidget->AddToViewport();
	}
}

FReply UD1ItemEntryWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (HoverWidget)
	{
		HoverWidget->SetPosition(InMouseEvent.GetScreenSpacePosition());
		return FReply::Handled();
	}
	
	return Reply;
}

void UD1ItemEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	Image_Hover->SetVisibility(ESlateVisibility::Hidden);

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

void UD1ItemEntryWidget::RefreshUI(UD1ItemInstance* NewItemInstance, int32 NewItemCount)
{
	if (NewItemInstance == nullptr || NewItemCount < 1)
		return;
	
	ItemInstance = NewItemInstance;
	ItemCount = NewItemCount;

	const UD1ItemTemplate& ItemTemplate = UD1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
	Image_Icon->SetBrushFromTexture(ItemTemplate.IconTexture, true);
	Text_Count->SetText(ItemCount <= 1 ? FText::GetEmpty() : FText::AsNumber(ItemCount));

	UTexture2D* RarityTexture = UD1UIData::Get().GetEntryRarityTexture(ItemInstance->GetItemRarity());
	Image_RarityCover->SetBrushFromTexture(RarityTexture, true);
}

void UD1ItemEntryWidget::RefreshItemCount(int32 NewItemCount)
{
	if (NewItemCount < 1)
		return;
	
	ItemCount = NewItemCount;
	Text_Count->SetText(ItemCount <= 1 ? FText::GetEmpty() : FText::AsNumber(ItemCount));
}
