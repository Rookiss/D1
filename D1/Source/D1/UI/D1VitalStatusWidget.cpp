#include "D1VitalStatusWidget.h"

#include "D1GameplayTags.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1VitalStatusWidget)

UD1VitalStatusWidget::UD1VitalStatusWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1VitalStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindAttributeChangedDelegate(D1GameplayTags::Attribute_Primary_Health, UD1AttributeSet::StaticClass(), this, &ThisClass::HandleHealthChanged);
	BindAttributeChangedDelegate(D1GameplayTags::Attribute_Primary_MaxHealth, UD1AttributeSet::StaticClass(), this, &ThisClass::HandleMaxHealthChanged);
	BindAttributeChangedDelegate(D1GameplayTags::Attribute_Primary_Mana, UD1AttributeSet::StaticClass(), this, &ThisClass::HandleManaChanged);
	BindAttributeChangedDelegate(D1GameplayTags::Attribute_Primary_MaxMana, UD1AttributeSet::StaticClass(), this, &ThisClass::HandleMaxManaChanged);
}

void UD1VitalStatusWidget::HandleHealthChanged(float NewValue)
{
	CachedHealth = NewValue;
	ProgressBar_Health->SetPercent(UKismetMathLibrary::SafeDivide(CachedHealth, CachedMaxHealth));
}

void UD1VitalStatusWidget::HandleMaxHealthChanged(float NewValue)
{
	CachedMaxHealth = NewValue;
	ProgressBar_Health->SetPercent(UKismetMathLibrary::SafeDivide(CachedHealth, CachedMaxHealth));
}

void UD1VitalStatusWidget::HandleManaChanged(float NewValue)
{
	CachedMana = NewValue;
	ProgressBar_Mana->SetPercent(UKismetMathLibrary::SafeDivide(CachedMana, CachedMaxMana));
}

void UD1VitalStatusWidget::HandleMaxManaChanged(float NewValue)
{
	CachedMaxMana = NewValue;
	ProgressBar_Mana->SetPercent(UKismetMathLibrary::SafeDivide(CachedMana, CachedMaxMana));
}
