#include "D1DamageExecutionCalculation.h"

#include "AbilitySystem/Attributes/D1MonsterSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1DamageExecutionCalculation)

struct FDamageStatics
{
public:
	FDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UD1MonsterSet, BaseDamage, Source, true);
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UD1PrimarySet, Strength, Source, true);
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UD1PrimarySet, Armor, Target, true);
	}

public:
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseDamage);
	// DECLARE_ATTRIBUTE_CAPTUREDEF(Strength);
	// DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
};

static const FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

UD1DamageExecutionCalculation::UD1DamageExecutionCalculation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef);
	// RelevantAttributesToCapture.Add(DamageStatics().StrengthDef);
	// RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
}

void UD1DamageExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluateParameters, BaseDamage);

	// TODO: Calculate Final Damage (Based on Attributes)
	const float DamageDone = FMath::Max(0.f, BaseDamage * 999.f);
	if (DamageDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UD1MonsterSet::GetDamageAttribute(), EGameplayModOp::Additive, DamageDone));
	}
#endif // #if WITH_SERVER_CODE
}
