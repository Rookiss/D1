#include "D1SkillStatHoverEntryWidget.h"

#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1SkillStatHoverEntryWidget)

UD1SkillStatHoverEntryWidget::UD1SkillStatHoverEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1SkillStatHoverEntryWidget::RefreshUI(FText Name, FText Description)
{
	Text_Name->SetText(Name);
	Text_Description->SetText(Description);

	PlayAnimationForward(Animation_FadeIn);
}
