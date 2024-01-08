#pragma once

#include "AbilitySystemGlobals.h"

#include "D1AbilitySystemGlobals.generated.h"

UCLASS()
class UD1AbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	
public:
	UD1AbilitySystemGlobals(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
