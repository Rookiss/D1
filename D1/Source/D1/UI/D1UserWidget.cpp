#include "D1UserWidget.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/D1AttributeSet.h"

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

	if (UD1AbilitySystemComponent* D1ASC = Cast<UD1AbilitySystemComponent>(AbilitySystemComponent))
	{
		D1ASC->AbilityChangedDelegate.Remove(AbilityDelegateHandle);

		for (const auto& Pair : AttributeDelegateHandles)
		{
			D1ASC->GetGameplayAttributeValueChangeDelegate(Pair.Key).Remove(Pair.Value);
		}
	}
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

void UD1UserWidget::BindAttributeChangedDelegates()
{
	BindUnitedAttributeChangedDelegates();
	BindSeparatedAttributeChangedDelegates();
}

void UD1UserWidget::BindUnitedAttributeChangedDelegates()
{
	for (const auto& Pair : WatchingAttributeTagToSetClass)
	{
		const FGameplayTag& AttributeTag = Pair.Key;
		const TSubclassOf<UD1AttributeSet> AttributeSetClass = Pair.Value;
		
		if (const UD1AttributeSet* AttributeSet = Cast<UD1AttributeSet>(AbilitySystemComponent->GetAttributeSet(AttributeSetClass)))
		{
			if (const FGameplayAttribute* Attribute = AttributeSet->TagToAttribute.Find(AttributeTag))
			{
				OnAttributeChanged(AttributeTag, Attribute->GetNumericValue(AttributeSet));
				FDelegateHandle Handle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(*Attribute).AddLambda([this, AttributeTag](const FOnAttributeChangeData& Data)
				{
					OnAttributeChanged(AttributeTag, Data.NewValue);
				});
				AttributeDelegateHandles.Emplace(*Attribute, Handle);
			}
		}
	}
}

void UD1UserWidget::BindSeparatedAttributeChangedDelegates()
{
	// TODO: Remove Handles
}
