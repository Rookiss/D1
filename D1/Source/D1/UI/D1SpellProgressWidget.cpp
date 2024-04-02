#include "D1SpellProgressWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1SpellProgressWidget)

UD1SpellProgressWidget::UD1SpellProgressWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1SpellProgressWidget::ShowWidget(const FText& SpellName, float CastTime)
{
	PassedCastTime = 0.f;
	Text_SpellName->SetText(SpellName);
	ProgressBar_SpellProgress->SetPercent(0.f);
	
	GetWorld()->GetTimerManager().SetTimer(ProgressTimerHandle, [this, CastTime]()
	{
		PassedCastTime = FMath::Min(PassedCastTime + ProgressTickTime, CastTime);
		ProgressBar_SpellProgress->SetPercent(UKismetMathLibrary::SafeDivide(PassedCastTime, CastTime));

		if (PassedCastTime >= CastTime)
		{
			GetWorld()->GetTimerManager().ClearTimer(ProgressTimerHandle);
		}
	}, ProgressTickTime, true);
	
	SetVisibility(ESlateVisibility::Visible);
}

void UD1SpellProgressWidget::HideWidget()
{
	if (ProgressTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ProgressTimerHandle);
	}
	
	SetVisibility(ESlateVisibility::Collapsed);
}
