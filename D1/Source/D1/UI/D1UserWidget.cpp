#include "D1UserWidget.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/D1AttributeSet.h"
#include "AbilitySystem/Attributes/D1PrimarySet.h"

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
	for (const auto& Pair : WatchingAttributes)
	{
		const FGameplayTag& AttributeTag = Pair.Key;
		const TSubclassOf<UD1AttributeSet> AttributeSetClass = Pair.Value;
		
		if (const UD1AttributeSet* AttributeSet = Cast<UD1AttributeSet>(AbilitySystemComponent->GetAttributeSet(AttributeSetClass)))
		{
			if (const auto& AttributeFunc = AttributeSet->GetAttributeFuncByTag(AttributeTag))
			{
				const FGameplayAttribute& Attribute = AttributeFunc();
				OnAttributeChanged(AttributeTag, Attribute.GetNumericValue(AttributeSet));
				FDelegateHandle Handle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddLambda(
				[this, AttributeTag](const FOnAttributeChangeData& Data)
				{
					OnAttributeChanged(AttributeTag, Data.NewValue);
				});
				AttributeDelegateHandles.Emplace(AttributeTag, AttributeSetClass, Handle);
			}
		}
	}
}

void UD1UserWidget::UnbindAttributeChangedDelegates()
{
	for (const auto& Tuple : AttributeDelegateHandles)
	{
		const FGameplayTag& AttributeTag = Tuple.Get<0>();
		const TSubclassOf<UD1AttributeSet> AttributeSetClass = Tuple.Get<1>();
		const FDelegateHandle& DelegateHandle = Tuple.Get<2>();
		
		if (const UD1AttributeSet* AttributeSet = Cast<UD1AttributeSet>(AbilitySystemComponent->GetAttributeSet(AttributeSetClass)))
		{
			if (const auto& AttributeFunc = AttributeSet->GetAttributeFuncByTag(AttributeTag))
			{
				AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeFunc()).Remove(DelegateHandle);
			}
		}
	}
	AttributeDelegateHandles.Empty();
}
