#include "D1GameplayAbility_Knockback.h"

#include "D1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/LyraCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Knockback)

UD1GameplayAbility_Knockback::UD1GameplayAbility_Knockback(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    ActivationPolicy = ELyraAbilityActivationPolicy::Manual;
	ActivationGroup = ELyraAbilityActivationGroup::Exclusive_Replaceable;
	bRetriggerInstancedAbility = true;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;

	AbilityTags.AddTag(D1GameplayTags::Ability_Knockback);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_Knockback);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = D1GameplayTags::GameplayEvent_Knockback;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UD1GameplayAbility_Knockback::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr || TriggerEventData->Instigator == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo())
	{
		LyraPlayerController->SetIgnoreMoveInput(true);

		if (ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo())
		{
			LyraCharacter->bUseControllerRotationYaw = false;
		}
	}

	CachedStunDuration = TriggerEventData->EventMagnitude;
	
	FVector WorldDirection = TriggerEventData->Instigator->GetActorForwardVector();
	if (UAbilityTask_ApplyRootMotionConstantForce* RootMotionForceTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this, TEXT("KnockbackForce"), WorldDirection, KnockbackStrength, KnockbackDuration,
		true, KnockbackStrengthCurve, ERootMotionFinishVelocityMode::ClampVelocity, FVector::ZeroVector, 100.f, true))
	{
		RootMotionForceTask->OnFinish.AddDynamic(this, &ThisClass::OnKnockbackFinished);
		RootMotionForceTask->ReadyForActivation();
	}

	UAnimMontage* SelectedMontage = SelectDirectionalMontage(TriggerEventData->Instigator, GetAvatarActorFromActorInfo());
	if (UAbilityTask_PlayMontageAndWait* KnockbackMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("KnockbackMontage"), SelectedMontage, 1.f, NAME_None, true))
	{
		KnockbackMontageTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Knockback::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ALyraPlayerController* LyraPC = GetLyraPlayerControllerFromActorInfo())
	{
		LyraPC->SetIgnoreMoveInput(false);

		if (ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo())
		{
			LyraCharacter->bUseControllerRotationYaw = true;
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

UAnimMontage* UD1GameplayAbility_Knockback::SelectDirectionalMontage(const AActor* Source, const AActor* Target) const
{
	if (Source == nullptr || Target == nullptr)
		return nullptr;

	const FRotator& SourceRotator = UKismetMathLibrary::Conv_VectorToRotator(Source->GetActorForwardVector());
	const FRotator& TargetRotator = UKismetMathLibrary::Conv_VectorToRotator(Target->GetActorForwardVector());

	const FRotator& DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(SourceRotator, TargetRotator);
	float YawAbs = FMath::Abs(DeltaRotator.Yaw);

	UAnimMontage* SelectedMontage;
	
	if (YawAbs < ForwardThreshold)
	{
		SelectedMontage = KnockbackBackwardMontage;
	}
	else if (YawAbs > BackwardThreshold)
	{
		SelectedMontage = KnockbackForwardMontage;
	}
	else if (DeltaRotator.Yaw < 0.f)
	{
		SelectedMontage = KnockbackLeftMontage;
	}
	else
	{
		SelectedMontage = KnockbackRightMontage;
	}

	return SelectedMontage;
}

void UD1GameplayAbility_Knockback::OnKnockbackFinished()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::ApplyStun, StunApplyDelay, false);
}

void UD1GameplayAbility_Knockback::ApplyStun()
{
	FGameplayEventData EventData;
	EventData.EventMagnitude = CachedStunDuration;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActorFromActorInfo(), D1GameplayTags::GameplayEvent_Stun, EventData);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
