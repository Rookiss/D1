#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "D1ActiveEffectDurationExecution.generated.h"

UCLASS()
class UD1ActiveEffectDurationExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UD1ActiveEffectDurationExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
