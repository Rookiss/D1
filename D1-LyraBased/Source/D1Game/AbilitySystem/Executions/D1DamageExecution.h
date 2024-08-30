#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "D1DamageExecution.generated.h"

UCLASS()
class UD1DamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UD1DamageExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
