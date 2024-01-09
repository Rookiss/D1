#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "D1HealExecutionCalculation.generated.h"

UCLASS()
class UD1HealExecutionCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UD1HealExecutionCalculation(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
