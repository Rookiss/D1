#include "D1VitalExecution.h"

#include "AbilitySystem/Attributes/D1VitalSet.h"
#include "AbilitySystem/Attributes/D1CombatSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1VitalExecution)

struct FVitalStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseHealthDef;
	FGameplayEffectAttributeCaptureDefinition BaseManaDef;
	FGameplayEffectAttributeCaptureDefinition BaseStaminaDef;

	FVitalStatics()
	{
		BaseHealthDef = FGameplayEffectAttributeCaptureDefinition(UD1CombatSet::GetBaseHealthAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		BaseManaDef = FGameplayEffectAttributeCaptureDefinition(UD1CombatSet::GetBaseManaAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		BaseStaminaDef = FGameplayEffectAttributeCaptureDefinition(UD1CombatSet::GetBaseStaminaAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FVitalStatics& HealStatics()
{
	static FVitalStatics Statics;
	return Statics;
}

UD1VitalExecution::UD1VitalExecution()
{
	RelevantAttributesToCapture.Add(HealStatics().BaseHealthDef);
	RelevantAttributesToCapture.Add(HealStatics().BaseManaDef);
	RelevantAttributesToCapture.Add(HealStatics().BaseStaminaDef);
}

void UD1VitalExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseHealth = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseHealthDef, EvaluateParameters, BaseHealth);

	float BaseMana = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseManaDef, EvaluateParameters, BaseMana);
	
	float BaseStamina = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseStaminaDef, EvaluateParameters, BaseStamina);

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UD1VitalSet::GetIncomingHealthAttribute(), EGameplayModOp::Additive, BaseHealth));
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UD1VitalSet::GetIncomingManaAttribute(), EGameplayModOp::Additive, BaseMana));
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UD1VitalSet::GetIncomingStaminaAttribute(), EGameplayModOp::Additive, BaseStamina));
	
#endif // #if WITH_SERVER_CODE
}
