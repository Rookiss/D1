#include "D1SpellProgressWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1SpellProgressWidget)

UD1SpellProgressWidget::UD1SpellProgressWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1SpellProgressWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (GetVisibility() == ESlateVisibility::Visible && PassedCastTime < TargetCastTime)
	{
		PassedCastTime = FMath::Min(PassedCastTime + InDeltaTime, TargetCastTime);
		ProgressBar_SpellProgress->SetPercent(UKismetMathLibrary::SafeDivide(PassedCastTime, TargetCastTime));
	}
}

void UD1SpellProgressWidget::ShowWidget(const FText& SpellName, float CastTime)
{
	PassedCastTime = 0.f;
	TargetCastTime = CastTime;
	
	Text_SpellName->SetText(SpellName);
	ProgressBar_SpellProgress->SetPercent(0.f);
	
	ProgressBar_SpellProgress->SetFillColorAndOpacity(DefaultColor);
	SetVisibility(ESlateVisibility::Visible);
}

void UD1SpellProgressWidget::NotifyWidget()
{
	ProgressBar_SpellProgress->SetFillColorAndOpacity(NotifyColor);
}

void UD1SpellProgressWidget::HideWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
