#include "D1ClassSkillEntryWidget.h"

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ClassSkillEntryWidget)

UD1ClassSkillEntryWidget::UD1ClassSkillEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ClassSkillEntryWidget::InitializeUI(const TSubclassOf<ULyraGameplayAbility>& AbilityClass)
{
	if (ULyraGameplayAbility* Ability = AbilityClass.GetDefaultObject())
	{
		Image_Skill->SetBrushFromTexture(Ability->Icon);
		Text_SkillName->SetText(Ability->Name);
		Text_SkillDescription->SetText(Ability->Description);
	}
}
