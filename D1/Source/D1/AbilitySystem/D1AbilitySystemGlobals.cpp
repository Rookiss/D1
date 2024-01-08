#include "D1AbilitySystemGlobals.h"

#include "D1GameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AbilitySystemGlobals)

UD1AbilitySystemGlobals::UD1AbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

FGameplayEffectContext* UD1AbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FD1GameplayEffectContext();
}
