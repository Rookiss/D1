#include "D1HealExecutionCalculation.h"

#include "AbilitySystem/Attributes/D1PrimarySet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1HealExecutionCalculation)

struct FHealStatics
{
public:
	FHealStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UD1PrimarySet, BaseHeal, Source, true);
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UD1PrimarySet, Strength, Source, false);
		// DEFINE_ATTRIBUTE_CAPTUREDEF(UD1PrimarySet, Armor, Target, false);
	}

public:
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseHeal);
	// DECLARE_ATTRIBUTE_CAPTUREDEF(Strength);
	// DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
};

static const FHealStatics& HealStatics()
{
	static FHealStatics Statics;
	return Statics;
	
}

UD1HealExecutionCalculation::UD1HealExecutionCalculation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RelevantAttributesToCapture.Add(HealStatics().BaseHealDef);
	// RelevantAttributesToCapture.Add(DamageStatics().StrengthDef);
	// RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
}

void UD1HealExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;
	
	float BaseHeal = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseHealDef, EvaluateParameters, BaseHeal);

	// TODO: Calculate Final Heal (Based on Attributes)
	const float HealDone = FMath::Max(0.f, BaseHeal * 999.f);
	if (HealDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UD1PrimarySet::GetHealAttribute(), EGameplayModOp::Additive, HealDone));
	}
#endif // #if WITH_SERVER_CODE
}
