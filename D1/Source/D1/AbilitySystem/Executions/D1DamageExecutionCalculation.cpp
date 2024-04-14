#include "D1DamageExecutionCalculation.h"

#include "D1GameplayTags.h"
#include "D1LogChannels.h"
#include "AbilitySystem/Attributes/D1AttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1DamageExecutionCalculation)

struct FDamageStatics
{
public:
	FDamageStatics()
	{
		PhysicalDamageDef = FGameplayEffectAttributeCaptureDefinition(UD1AttributeSet::GetPhysicalDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		MagicalDamageDef = FGameplayEffectAttributeCaptureDefinition(UD1AttributeSet::GetMagicalDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		BaseDefenseDef = FGameplayEffectAttributeCaptureDefinition(UD1AttributeSet::GetBaseDefenseAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		StrengthDef = FGameplayEffectAttributeCaptureDefinition(UD1AttributeSet::GetStrengthAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		WillDef = FGameplayEffectAttributeCaptureDefinition(UD1AttributeSet::GetWillAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}

public:
	FGameplayEffectAttributeCaptureDefinition PhysicalDamageDef;
	FGameplayEffectAttributeCaptureDefinition MagicalDamageDef;
	FGameplayEffectAttributeCaptureDefinition BaseDefenseDef;
	FGameplayEffectAttributeCaptureDefinition StrengthDef;
	FGameplayEffectAttributeCaptureDefinition WillDef;
};

static const FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

UD1DamageExecutionCalculation::UD1DamageExecutionCalculation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicalDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().BaseDefenseDef);
	RelevantAttributesToCapture.Add(DamageStatics().StrengthDef);
	RelevantAttributesToCapture.Add(DamageStatics().WillDef);
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
	
	float FinalDamage = 0;
	if (Spec.GetDynamicAssetTags().HasTagExact(D1GameplayTags::Attack_Physical))
	{
		float PhysicalDamage = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PhysicalDamageDef, EvaluateParameters, PhysicalDamage);
		
		float Strength = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().StrengthDef, EvaluateParameters, Strength);
		
		FinalDamage = PhysicalDamage + Strength;
	}
	else if (Spec.GetDynamicAssetTags().HasTagExact(D1GameplayTags::Attack_Magical))
	{
		float MagicalDamage = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().MagicalDamageDef, EvaluateParameters, MagicalDamage);
		
		float Will = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().WillDef, EvaluateParameters, Will);

		FinalDamage = MagicalDamage + Will;
	}
	else
	{
		UE_LOG(LogD1AbilitySystem, Error, TEXT("Attack Type is Invaild."));
	}
	
	if (FinalDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UD1AttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, FinalDamage));
	}
#endif // #if WITH_SERVER_CODE
}
