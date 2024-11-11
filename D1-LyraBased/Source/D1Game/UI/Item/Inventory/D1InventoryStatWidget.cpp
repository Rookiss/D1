#include "D1InventoryStatWidget.h"

#include "Data/D1UIData.h"
#include "UI/Item/D1SkillStatHoverWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InventoryStatWidget)

UD1InventoryStatWidget::UD1InventoryStatWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1InventoryStatWidget::NativeDestruct()
{
	if (HoverWidget)
	{
		HoverWidget->RemoveFromParent();
		HoverWidget = nullptr;
	}
	
	Super::NativeDestruct();
}

void UD1InventoryStatWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	const FD1UIInfo& UIInfo = UD1UIData::Get().GetTagUIInfo(StatTag);
	if (UIInfo.Title.IsEmpty() == false && UIInfo.Content.IsEmpty() == false)
	{
		if (HoverWidget == nullptr)
		{
			TSubclassOf<UD1SkillStatHoverWidget> HoverWidgetClass = UD1UIData::Get().SkillStatHoverWidget;
			HoverWidget = CreateWidget<UD1SkillStatHoverWidget>(GetOwningPlayer(), HoverWidgetClass);
		}

		if (HoverWidget)
		{
		
			HoverWidget->RefreshUI(UIInfo.Title, UIInfo.Content);
			HoverWidget->AddToViewport();
		}
	}
}

FReply UD1InventoryStatWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (HoverWidget)
	{
		HoverWidget->SetPosition(InMouseEvent.GetScreenSpacePosition());
		return FReply::Handled();
	}
	
	return Reply;
}

void UD1InventoryStatWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (HoverWidget)
	{
		HoverWidget->RemoveFromParent();
		HoverWidget = nullptr;
	}
}
