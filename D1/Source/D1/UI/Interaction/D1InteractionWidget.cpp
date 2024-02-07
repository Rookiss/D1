#include "D1InteractionWidget.h"

#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InteractionWidget)

UD1InteractionWidget::UD1InteractionWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1InteractionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
	CircularMaterialInstance = Image_Circular_Fill->GetDynamicMaterial();
	
	NumberFormattingOptions.MinimumFractionalDigits = 1;
	NumberFormattingOptions.MaximumFractionalDigits = 1;
}

void UD1InteractionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (LeftHoldTime >= 0.f)
	{
		LeftHoldTime -= InDeltaTime;
		Text_HoldTime->SetText(FText::AsNumber(LeftHoldTime, &NumberFormattingOptions));
		CircularMaterialInstance->SetScalarParameterValue("Percent", LeftHoldTime / TotalHoldTime);
	}
}

void UD1InteractionWidget::ShowInteractionPress(const FText& InteractionTitle, const FText& InteractionContent)
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
	WidgetSwitcher->SetActiveWidgetIndex(0);

	Text_InteractionTitle->SetText(InteractionTitle);
	Text_InteractionContent->SetText(InteractionContent);
}

void UD1InteractionWidget::ShowInteractionProgress(float HoldTime)
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
	WidgetSwitcher->SetActiveWidgetIndex(1);

	TotalHoldTime = HoldTime;
	LeftHoldTime = HoldTime;
}

void UD1InteractionWidget::HideInteractionWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
