#include "D1WorldMapWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1WorldMapWidget)

UD1WorldMapWidget::UD1WorldMapWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1WorldMapWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UD1WorldMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FReply UD1WorldMapWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply UD1WorldMapWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UD1WorldMapWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
}

FReply UD1WorldMapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
