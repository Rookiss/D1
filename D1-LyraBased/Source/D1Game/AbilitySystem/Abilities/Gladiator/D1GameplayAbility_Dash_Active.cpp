#include "D1GameplayAbility_Dash_Active.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "D1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/LyraCharacter.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Dash_Active)

UD1GameplayAbility_Dash_Active::UD1GameplayAbility_Dash_Active(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    ActivationPolicy = ELyraAbilityActivationPolicy::Manual;

	AbilityTags.AddTag(D1GameplayTags::Ability_Dash_Active);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_Dash);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = D1GameplayTags::Ability_Dash_Active;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UD1GameplayAbility_Dash_Active::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo())
	{
		LyraPlayerController->SetIgnoreMoveInput(true);
	}
	
	const FHitResult& HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TriggerEventData->TargetData, 0);
	ED1Direction Direction = (ED1Direction)HitResult.Item;
	
	UAnimMontage* SelectedMontage = SelectDirectionalMontage(Direction);
	if (UAbilityTask_PlayMontageAndWait* DashMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("DashMontage"), SelectedMontage, 1.f, NAME_None, true))
	{
		DashMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		DashMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		DashMontageTask->ReadyForActivation();
	}

	FVector MovementVector = HitResult.Normal;
	if (UAbilityTask_ApplyRootMotionConstantForce* RootMotionForceTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this, TEXT("DashForce"), MovementVector, DashStrength, DashDuration, true, nullptr, ERootMotionFinishVelocityMode::ClampVelocity, FVector::ZeroVector, 100.f, true))
	{
		RootMotionForceTask->OnFinish.AddDynamic(this, &ThisClass::OnDashFinished);
		RootMotionForceTask->ReadyForActivation();
	}
}

void UD1GameplayAbility_Dash_Active::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo())
	{
		LyraPlayerController->SetIgnoreMoveInput(false);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

UAnimMontage* UD1GameplayAbility_Dash_Active::SelectDirectionalMontage(ED1Direction MovementDirection) const
{
	UAnimMontage* AnimMontage = nullptr;
	switch (MovementDirection)
	{
	case ED1Direction::Right:		AnimMontage = RightMontage;		break;
	case ED1Direction::Backward:	AnimMontage = BackwardMontage;	break;
	case ED1Direction::Left:		AnimMontage = LeftMontage;		break;
	}
	return AnimMontage;
}

void UD1GameplayAbility_Dash_Active::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UD1GameplayAbility_Dash_Active::OnDashFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
