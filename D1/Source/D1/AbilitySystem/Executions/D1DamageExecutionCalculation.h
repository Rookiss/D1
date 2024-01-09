#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "D1DamageExecutionCalculation.generated.h"

UCLASS()
class UD1DamageExecutionCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UD1DamageExecutionCalculation(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
