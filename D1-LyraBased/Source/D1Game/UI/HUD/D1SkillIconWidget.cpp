#include "D1SkillIconWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "D1GameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1SkillIconWidget)

UD1SkillIconWidget::UD1SkillIconWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1SkillIconWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	// if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn()))
	// {
	// 	for (const FGameplayAbilitySpec& AbilitySpec : ASC->GetActivatableAbilities())
	// 	{
	// 		for (const FGameplayTag& AbilityTag : AbilitySpec.Ability->AbilityTags)
	// 		{
	// 			if (AbilityTag.MatchesTag(FGameplayTag::RequestGameplayTag("Ability")))
	// 			{
	// 				OnAbilityChanged(true, AbilityTag);
	// 				break;
	// 			}
	// 		}
	// 	}
	// 	AbilityDelegateHandle = D1ASC->AbilityChangedDelegate.AddUObject(this, &ThisClass::OnAbilityChanged);
	// }
}

void UD1SkillIconWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// TODO: Temp
}

void UD1SkillIconWidget::OnSkillIconClicked()
{
	
}
