#include "D1DamageExecution.h"

#include "D1LogChannels.h"
#include "AbilitySystem/Attributes/D1VitalSet.h"
#include "AbilitySystem/Attributes/D1CombatSet.h"
#include "AbilitySystem/LyraGameplayEffectContext.h"
#include "Teams/LyraTeamSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1DamageExecution)

struct FDamageStatics
{
public:
	FDamageStatics()
	{
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(UD1CombatSet::GetBaseDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		StrengthDef = FGameplayEffectAttributeCaptureDefinition(UD1CombatSet::GetStrengthAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		DefenseDef = FGameplayEffectAttributeCaptureDefinition(UD1CombatSet::GetDefenseAttribute(), EGameplayEffectAttributeCaptureSource::Target, true);
		DrainLifePercentDef = FGameplayEffectAttributeCaptureDefinition(UD1CombatSet::GetDrainLifePercentAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}

public:
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;
	FGameplayEffectAttributeCaptureDefinition StrengthDef;
	FGameplayEffectAttributeCaptureDefinition DefenseDef;
	FGameplayEffectAttributeCaptureDefinition DrainLifePercentDef;
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

UD1DamageExecution::UD1DamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().StrengthDef);
	RelevantAttributesToCapture.Add(DamageStatics().DefenseDef);
	RelevantAttributesToCapture.Add(DamageStatics().DrainLifePercentDef);
}

void UD1DamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FLyraGameplayEffectContext* TypedContext = FLyraGameplayEffectContext::ExtractEffectContext(Spec.GetContext());
	check(TypedContext);

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;
	
	float BaseDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluateParameters, BaseDamage);

	float Strength = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().StrengthDef, EvaluateParameters, Strength);

	float Defense = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseDef, EvaluateParameters, Defense);

	float DrainLifePercent = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DrainLifePercentDef, EvaluateParameters, DrainLifePercent);
	
	const float DamageDone = FMath::Max((BaseDamage + Strength) / (FMath::Pow(Defense, 0.3f)), 0.0f);
	
	if (DamageDone > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UD1VitalSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, DamageDone));

		if (UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent())
		{
			SourceASC->ApplyModToAttribute(UD1VitalSet::GetHealthAttribute(), EGameplayModOp::Additive, DamageDone * (DrainLifePercent / 100.f));
		}
	}
#endif // WITH_SERVER_CODE
}
