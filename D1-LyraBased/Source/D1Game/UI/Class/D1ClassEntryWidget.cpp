#include "D1ClassEntryWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "D1ClassSkillEntryWidget.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "Components/Button.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ClassEntryWidget)

UD1ClassEntryWidget::UD1ClassEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ClassEntryWidget::InitializeUI(const FClassEntry& ClassEntry)
{
	CachedClassEntry = ClassEntry;
	
	Image_Class->SetBrushFromTexture(ClassEntry.ClassIcon.LoadSynchronous());
	Text_ClassName->SetText(ClassEntry.ClassName);

	VerticalBox_SkillElements->ClearChildren();
	if (ULyraAbilitySet* AbilitySet = ClassEntry.ClassAbilitySet)
	{
		const TArray<FLyraAbilitySet_GameplayAbility>& AbilitySetAbilities = AbilitySet->GetGrantedGameplayAbilities();
		for (int i = 0; i < 2; i++)
		{
			if (AbilitySetAbilities.IsValidIndex(i))
			{
				const FLyraAbilitySet_GameplayAbility& AbilitySetAbility = AbilitySetAbilities[i];
				UD1ClassSkillEntryWidget* SkillEntryWidget = CreateWidget<UD1ClassSkillEntryWidget>(this, SkillEntryWidgetClass);
				SkillEntryWidget->InitializeUI(AbilitySetAbility.Ability);
				VerticalBox_SkillElements->AddChild(SkillEntryWidget);
			}
		}
	}

	Button_Class->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButtonClicked);
}

void UD1ClassEntryWidget::OnButtonClicked()
{
	
}
