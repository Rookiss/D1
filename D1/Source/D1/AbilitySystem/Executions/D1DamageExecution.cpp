#include "D1DamageExecution.h"

#include "AbilitySystem/Attributes/D1PrimarySet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1DamageExecution)

struct FDamageStatics
{
public:
	FDamageStatics()
	{
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UD1PrimarySet, Strength, Source, false);
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UD1PrimarySet, Armor, Target, false);
	}

public:
	// DECLARE_ATTRIBUTE_CAPTUREDEF(Strength);
	// DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
};

static const FDamageStatics& DamageStatics()
{
	static FDamageStatics DamageStatics;
	return DamageStatics;
}

UD1DamageExecution::UD1DamageExecution(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// RelevantAttributesToCapture.Add(DamageStatics().StrengthDef);
	// RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
}

void UD1DamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;
	
	// TODO: Calculate Damage (Based on Attributes)
	// float Strength;
	// ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().StrengthDef, EvaluateParameters, Strength);
	const float Damage = 999.f; // = FMath::Max(BaseDamage * DistanceAttenuation * PhysicalMaterialAttenuation * DamageInteractionAllowedMultiplier, 0.0f);

	if (Damage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UD1PrimarySet::GetDamageAttribute(), EGameplayModOp::Additive, Damage));
	}
}
