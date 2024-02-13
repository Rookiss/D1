#include "D1InteractionWidget.h"

#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Interaction/D1Interactable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1InteractionWidget)

UD1InteractionWidget::UD1InteractionWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1InteractionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HideInteractionWidget();
	
	CircularMaterialInstance = Image_Circular_Fill->GetDynamicMaterial();
	CircularMaterialInstance->SetScalarParameterValue("Percent", 1.f);
	
	NumberFormattingOptions.MinimumFractionalDigits = 1;
	NumberFormattingOptions.MaximumFractionalDigits = 1;
}

void UD1InteractionWidget::ShowInteractionPressWidget(const FText& InteractionTitle, const FText& InteractionContent)
{
	WidgetSwitcher->SetVisibility(ESlateVisibility::Visible);
	WidgetSwitcher->SetActiveWidgetIndex(0);

	Text_InteractionTitle->SetText(InteractionTitle);
	Text_InteractionContent->SetText(InteractionContent);
}

void UD1InteractionWidget::ShowInteractionDurationWidget(float HoldTime)
{
	WidgetSwitcher->SetVisibility(ESlateVisibility::Visible);
	WidgetSwitcher->SetActiveWidgetIndex(1);

	LeftHoldTime = HoldTime;
	TotalHoldTime = HoldTime;

	CircularMaterialInstance->SetScalarParameterValue("Percent", 1.f);
	GetWorld()->GetTimerManager().SetTimer(ProgressTimerHandle, [this]()
	{
		LeftHoldTime = FMath::Max(LeftHoldTime - ProgressTickTime, 0.f);
		Text_HoldTime->SetText(FText::AsNumber(LeftHoldTime, &NumberFormattingOptions));
		CircularMaterialInstance->SetScalarParameterValue("Percent", LeftHoldTime / TotalHoldTime);

		if (LeftHoldTime <= 0.f)
		{
			GetWorld()->GetTimerManager().ClearTimer(ProgressTimerHandle);
		}
	}, ProgressTickTime, true);
}

void UD1InteractionWidget::HideInteractionWidget()
{
	WidgetSwitcher->SetVisibility(ESlateVisibility::Collapsed);
}

bool UD1InteractionWidget::IsShowingDurationWidget() const
{
	return (WidgetSwitcher->GetVisibility() == ESlateVisibility::Visible && WidgetSwitcher->GetActiveWidgetIndex() == 1);
}
