#include "D1GameplayAbility_Stun.h"

#include "AIController.h"
#include "D1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/LyraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Stun)

UD1GameplayAbility_Stun::UD1GameplayAbility_Stun(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    ActivationPolicy = ELyraAbilityActivationPolicy::Manual;
	ActivationGroup = ELyraAbilityActivationGroup::Exclusive_Replaceable;
	bRetriggerInstancedAbility = true;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;

	AbilityTags.AddTag(D1GameplayTags::Ability_Stun);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_Stun);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = D1GameplayTags::GameplayEvent_Stun;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UD1GameplayAbility_Stun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	AController* Controller = GetControllerFromActorInfo();
	if (Controller == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	if (LyraCharacter == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	if (Controller->IsA(AAIController::StaticClass()))
	{
		if (UCharacterMovementComponent* CharacterMovement = LyraCharacter->GetCharacterMovement())
		{
			CharacterMovement->bUseControllerDesiredRotation = false;
		}
	}
	else
	{
		Controller->SetIgnoreMoveInput(true);
		LyraCharacter->bUseControllerRotationYaw = false;
	}

	if (UAbilityTask_PlayMontageAndWait* StunMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("StunMontage"), StunMontage, 1.f, NAME_None, true))
	{
		StunMontageTask->ReadyForActivation();
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}, TriggerEventData->EventMagnitude, false);
}

void UD1GameplayAbility_Stun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (AController* Controller = GetControllerFromActorInfo())
	{
		if (ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo())
		{
			if (Controller->IsA(AAIController::StaticClass()))
			{
				if (UCharacterMovementComponent* CharacterMovement = LyraCharacter->GetCharacterMovement())
				{
					CharacterMovement->bUseControllerDesiredRotation = true;
				}
			}
			else
			{
				Controller->SetIgnoreMoveInput(false);
				LyraCharacter->bUseControllerRotationYaw = true;
			}
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
