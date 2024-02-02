#include "D1DamageExecutionCalculation.h"

#include "D1GameplayTags.h"
#include "D1LogChannels.h"
#include "AbilitySystem/Attributes/D1MonsterSet.h"
#include "AbilitySystem/Attributes/D1PlayerSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1DamageExecutionCalculation)

struct FDamageStatics
{
public:
	FDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UD1MonsterSet, BaseDamage, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UD1MonsterSet, BaseDefense, Target, true);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UD1PlayerSet, Strength, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UD1PlayerSet, Will, Source, true);
	}

public:
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseDefense);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(Strength);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Will);
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

	float BaseDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluateParameters, BaseDamage);
	
	float FinalDamage = 0;
	if (Spec.GetDynamicAssetTags().HasTagExact(D1GameplayTags::Attack_Physical))
	{
		float Strength = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().StrengthDef, EvaluateParameters, Strength);
		
		FinalDamage = BaseDamage + Strength;
	}
	else if (Spec.GetDynamicAssetTags().HasTagExact(D1GameplayTags::Attack_Magical))
	{
		float Will = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().WillDef, EvaluateParameters, Will);

		FinalDamage = BaseDamage + Will;
	}
	else
	{
		UE_LOG(LogD1AbilitySystem, Error, TEXT("Attack Type is Invaild."));
	}
	
	if (FinalDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UD1MonsterSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, FinalDamage));
	}
#endif // #if WITH_SERVER_CODE
}
