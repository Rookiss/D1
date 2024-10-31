#include "D1DamageExecution.h"

#include "AbilitySystem/LyraAbilitySourceInterface.h"
#include "AbilitySystem/Attributes/D1VitalSet.h"
#include "AbilitySystem/Attributes/D1CombatSet.h"
#include "AbilitySystem/LyraGameplayEffectContext.h"
#include "Kismet/KismetMathLibrary.h"
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
		DamageReductionPercentDef = FGameplayEffectAttributeCaptureDefinition(UD1CombatSet::GetDamageReductionPercentAttribute(), EGameplayEffectAttributeCaptureSource::Target, true);
	}

public:
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;
	FGameplayEffectAttributeCaptureDefinition StrengthDef;
	FGameplayEffectAttributeCaptureDefinition DefenseDef;
	FGameplayEffectAttributeCaptureDefinition DrainLifePercentDef;
	FGameplayEffectAttributeCaptureDefinition DamageReductionPercentDef;
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
	RelevantAttributesToCapture.Add(DamageStatics().DamageReductionPercentDef);
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

	float DamageReductionPercent = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageReductionPercentDef, EvaluateParameters, DamageReductionPercent);

	const AActor* EffectCauser = TypedContext->GetEffectCauser();
	const FHitResult* HitActorResult = TypedContext->GetHitResult();
	AActor* HitActor = nullptr;
	
	if (HitActorResult)
	{
		const FHitResult& CurrHitResult = *HitActorResult;
		HitActor = CurrHitResult.HitObjectHandle.FetchActor();
	}
	
	float Damage = UKismetMathLibrary::SafeDivide(BaseDamage + Strength, FMath::Pow(FMath::Max(Defense, 1.f), 0.3f));

	// Apply rules for team damage
	float DamageInteractionAllowedMultiplier = 0.0f;
	if (HitActor)
	{
		ULyraTeamSubsystem* TeamSubsystem = HitActor->GetWorld()->GetSubsystem<ULyraTeamSubsystem>();
		if (ensure(TeamSubsystem))
		{
			DamageInteractionAllowedMultiplier = TeamSubsystem->CanCauseDamage(EffectCauser, HitActor) ? 1.f : 0.f;
		}
	}
	
	// Weak point damage calculation
	float PhysicalMaterialAttenuation = 1.0f;
	if (const ILyraAbilitySourceInterface* AbilitySource = TypedContext->GetAbilitySource())
	{
		if (const UPhysicalMaterial* PhysicalMaterial = TypedContext->GetPhysicalMaterial())
		{
			PhysicalMaterialAttenuation = AbilitySource->GetPhysicalMaterialAttenuation(PhysicalMaterial, SourceTags, TargetTags);
		}
	}
	
	float DamageDone = FMath::Max(Damage * PhysicalMaterialAttenuation * DamageInteractionAllowedMultiplier, 0.0f);
	DamageDone -= DamageDone * (DamageReductionPercent / 100.f);
	
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
