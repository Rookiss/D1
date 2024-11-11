#include "D1SkillStatHoverWidget.h"

#include "D1SkillStatHoverEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1SkillStatHoverWidget)

UD1SkillStatHoverWidget::UD1SkillStatHoverWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1SkillStatHoverWidget::RefreshUI(FText Name, FText Description)
{
	Widget_HoverEntry->RefreshUI(Name, Description);
}
