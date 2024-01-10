#include "D1AttributeSet.h"

#include "D1LogChannels.h"
#include "AbilitySystem/D1AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AttributeSet)

UD1AttributeSet::UD1AttributeSet()
{
	
}

TFunction<FGameplayAttribute()> UD1AttributeSet::GetAttributeFuncByTag(const FGameplayTag& AttributeTag) const
{
	if (const TFunction<FGameplayAttribute()>* FuncPtr = TagToAttributeFunc.Find(AttributeTag))
	{
		return *FuncPtr;
	}
	
	UE_LOG(LogD1AbilitySystem, Error, TEXT("Can't find Attribute Func By Tag [%s]."), *AttributeTag.ToString());
	return nullptr;
}

UD1AbilitySystemComponent* UD1AttributeSet::GetD1AbilitySystemComponent() const
{
	return Cast<UD1AbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
