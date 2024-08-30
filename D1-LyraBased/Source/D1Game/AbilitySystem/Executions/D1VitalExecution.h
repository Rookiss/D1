#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "D1VitalExecution.generated.h"

UCLASS()
class UD1VitalExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UD1VitalExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
