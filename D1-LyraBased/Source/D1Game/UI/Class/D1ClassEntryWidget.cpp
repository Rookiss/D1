#include "D1ClassEntryWidget.h"

#include "D1ClassSelectionWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "D1ClassSkillEntryWidget.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "Components/Button.h"
#include "Data/D1ClassData.h"
#include "Player/LyraPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ClassEntryWidget)

UD1ClassEntryWidget::UD1ClassEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ClassEntryWidget::InitializeUI(UD1ClassSelectionWidget* OwnerWidget, ECharacterClassType ClassType)
{
	CachedClassType = ClassType;
	CachedOwnerWidget = OwnerWidget;

	const FD1ClassInfoEntry& ClassEntry = UD1ClassData::Get().GetClassInfoEntry(ClassType);
	
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
	if (ALyraPlayerState* LyraPlayerState = Cast<ALyraPlayerState>(GetOwningPlayerState()))
	{
		LyraPlayerState->Server_SelectClass(CachedClassType);
	}

	if (UD1ClassSelectionWidget* ClassSelectionWidget = CachedOwnerWidget.Get())
	{
		ClassSelectionWidget->DeactivateWidget();
		ClassSelectionWidget = nullptr;
	}
}
