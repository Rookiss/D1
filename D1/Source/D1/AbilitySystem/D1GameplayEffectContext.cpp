#include "D1GameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayEffectContext)

FD1GameplayEffectContext::FD1GameplayEffectContext()
	: Super()
{
    
}

FGameplayEffectContext* FD1GameplayEffectContext::Duplicate() const
{
	FD1GameplayEffectContext* NewContext = new FD1GameplayEffectContext();
	*NewContext = *this;
	if (GetHitResult())
	{
		NewContext->AddHitResult(*GetHitResult(), true);
	}
	return NewContext;
}

UScriptStruct* FD1GameplayEffectContext::GetScriptStruct() const
{
	return FD1GameplayEffectContext::StaticStruct();
}

bool FD1GameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	return true;
}
