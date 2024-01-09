#include "D1UserWidget.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1UserWidget)

UD1UserWidget::UD1UserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1UserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningPlayerPawn()))
	{
		AbilitySystemComponent = ASC;
		BindAbilityChangedDelegate();
		BindAttributeChangedDelegates();
	}
}

void UD1UserWidget::NativeDestruct()
{
	Super::NativeDestruct();

	UnbindAbilityChangedDelegate();
	UnbindAttributeChangedDelegates();
}

void UD1UserWidget::BindAbilityChangedDelegate()
{
	if (UD1AbilitySystemComponent* D1ASC = Cast<UD1AbilitySystemComponent>(AbilitySystemComponent))
	{
		for (const FGameplayAbilitySpec& AbilitySpec : D1ASC->GetActivatableAbilities())
		{
			for (const FGameplayTag& AbilityTag : AbilitySpec.Ability->AbilityTags)
			{
				if (AbilityTag.MatchesTag(FGameplayTag::RequestGameplayTag("Ability")))
				{
					OnAbilityChanged(true, AbilityTag);
					break;
				}
			}
		}
		AbilityDelegateHandle = D1ASC->AbilityChangedDelegate.AddUObject(this, &ThisClass::OnAbilityChanged);
	}
}

void UD1UserWidget::UnbindAbilityChangedDelegate()
{
	if (UD1AbilitySystemComponent* D1ASC = Cast<UD1AbilitySystemComponent>(AbilitySystemComponent))
	{
		D1ASC->AbilityChangedDelegate.Remove(AbilityDelegateHandle);
		AbilityDelegateHandle.Reset();
	}
}

void UD1UserWidget::BindAttributeChangedDelegates()
{
	
}

void UD1UserWidget::UnbindAttributeChangedDelegates()
{
	
}
