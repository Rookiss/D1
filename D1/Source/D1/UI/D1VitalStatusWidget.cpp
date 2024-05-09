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
	CachedCurrentHealth = CachedTargetHealth = AbilitySystemComponent->GetNumericAttribute(UD1AttributeSet::GetHealthAttribute());
	
	BindAttributeChangedDelegate(D1GameplayTags::Attribute_Primary_MaxHealth, UD1AttributeSet::StaticClass(), this, &ThisClass::HandleMaxHealthChanged);
	
	BindAttributeChangedDelegate(D1GameplayTags::Attribute_Primary_Stamina, UD1AttributeSet::StaticClass(), this, &ThisClass::HandleStaminaChanged);
	CachedCurrentStamina = CachedTargetStamina = AbilitySystemComponent->GetNumericAttribute(UD1AttributeSet::GetStaminaAttribute());
	
	BindAttributeChangedDelegate(D1GameplayTags::Attribute_Primary_MaxStamina, UD1AttributeSet::StaticClass(), this, &ThisClass::HandleMaxStaminaChanged);
}

void UD1VitalStatusWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	CachedCurrentHealth = UKismetMathLibrary::FInterpTo(CachedCurrentHealth, CachedTargetHealth, InDeltaTime, 8.f);
	ProgressBar_Health->SetPercent(UKismetMathLibrary::SafeDivide(CachedCurrentHealth, CachedMaxHealth));

	CachedCurrentStamina = UKismetMathLibrary::FInterpTo(CachedCurrentStamina, CachedTargetStamina, InDeltaTime, 8.f);
	ProgressBar_Stamina->SetPercent(UKismetMathLibrary::SafeDivide(CachedCurrentStamina, CachedMaxStamina));
}

void UD1VitalStatusWidget::HandleHealthChanged(float NewValue)
{
	CachedTargetHealth = NewValue;
}

void UD1VitalStatusWidget::HandleMaxHealthChanged(float NewValue)
{
	CachedMaxHealth = NewValue;
}

void UD1VitalStatusWidget::HandleStaminaChanged(float NewValue)
{
	CachedTargetStamina = NewValue;
}

void UD1VitalStatusWidget::HandleMaxStaminaChanged(float NewValue)
{
	CachedMaxStamina = NewValue;
}
