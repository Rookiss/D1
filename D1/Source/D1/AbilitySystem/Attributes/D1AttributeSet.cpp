#include "D1AttributeSet.h"

#include "AbilitySystem/D1AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AttributeSet)

UD1AttributeSet::UD1AttributeSet()
{
	
}

UD1AbilitySystemComponent* UD1AttributeSet::GetD1AbilitySystemComponent() const
{
	return Cast<UD1AbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
