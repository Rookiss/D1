#include "D1ClassEntryWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "CommonUIUtils.h"
#include "D1ClassSelectionWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "D1ClassSkillEntryWidget.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Components/Button.h"
#include "Player/LyraPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1ClassEntryWidget)

UD1ClassEntryWidget::UD1ClassEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1ClassEntryWidget::InitializeUI(UD1ClassSelectionWidget* OwnerWidget, int32 ClassIndex)
{
	CachedClassIndex = ClassIndex;
	CachedOwnerWidget = OwnerWidget;

	const FClassEntry& ClassEntry = UD1ClassData::Get().GetClassEntry(ClassIndex);
	
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
		LyraPlayerState->Server_SelectClass(CachedClassIndex);
	}

	if (UD1ClassSelectionWidget* ClassSelectionWidget = CachedOwnerWidget.Get())
	{
		ClassSelectionWidget->DeactivateWidget();
		ClassSelectionWidget = nullptr;
	}
}
