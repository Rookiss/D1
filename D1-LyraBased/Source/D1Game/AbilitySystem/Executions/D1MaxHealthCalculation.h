#pragma once

#include "GameplayModMagnitudeCalculation.h"
#include "D1MaxHealthCalculation.generated.h"

UCLASS()
class UD1MaxHealthCalculation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UD1MaxHealthCalculation();

protected:
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition VigorDef;
};
