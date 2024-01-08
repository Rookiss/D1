#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "D1DamageExecution.generated.h"

UCLASS()
class UD1DamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UD1DamageExecution(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
