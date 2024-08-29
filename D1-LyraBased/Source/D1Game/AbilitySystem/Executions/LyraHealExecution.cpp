#include "LyraHealExecution.h"
#include "AbilitySystem/Attributes/LyraHealthSet.h"
#include "AbilitySystem/Attributes/LyraCombatSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraHealExecution)

struct FHealStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseHealHealthDef;
	FGameplayEffectAttributeCaptureDefinition BaseHealManaDef;
	FGameplayEffectAttributeCaptureDefinition BaseHealStaminaDef;

	FHealStatics()
	{
		BaseHealHealthDef = FGameplayEffectAttributeCaptureDefinition(ULyraCombatSet::GetBaseHealHealthAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		BaseHealManaDef = FGameplayEffectAttributeCaptureDefinition(ULyraCombatSet::GetBaseHealManaAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		BaseHealStaminaDef = FGameplayEffectAttributeCaptureDefinition(ULyraCombatSet::GetBaseHealStaminaAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FHealStatics& HealStatics()
{
	static FHealStatics Statics;
	return Statics;
}

ULyraHealExecution::ULyraHealExecution()
{
	RelevantAttributesToCapture.Add(HealStatics().BaseHealHealthDef);
	RelevantAttributesToCapture.Add(HealStatics().BaseHealManaDef);
	RelevantAttributesToCapture.Add(HealStatics().BaseHealStaminaDef);
}

void ULyraHealExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseHealHealth = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseHealHealthDef, EvaluateParameters, BaseHealHealth);

	float BaseHealMana = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseHealManaDef, EvaluateParameters, BaseHealMana);
	
	float BaseHealStamina = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseHealStaminaDef, EvaluateParameters, BaseHealStamina);

	const float HealingHealthDone = FMath::Max(0.0f, BaseHealHealth);
	const float HealingManaDone = FMath::Max(0.0f, BaseHealMana);
	const float HealingStaminaDone = FMath::Max(0.0f, BaseHealStamina);

	if (HealingHealthDone > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(ULyraHealthSet::GetIncomingHealAttribute(), EGameplayModOp::Additive, HealingHealthDone));
	}

	if (HealingManaDone > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(ULyraCombatSet::GetManaAttribute(), EGameplayModOp::Additive, HealingManaDone));
	}

	if (HealingStaminaDone > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(ULyraCombatSet::GetStaminaAttribute(), EGameplayModOp::Additive, HealingStaminaDone));
	}
#endif // #if WITH_SERVER_CODE
}
