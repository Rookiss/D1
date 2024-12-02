#include "D1GameplayAbility_Sprint_Active.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "D1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystem/Abilities/Tasks/D1AbilityTask_WaitForTick.h"
#include "Character/LyraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Sprint_Active)

UD1GameplayAbility_Sprint_Active::UD1GameplayAbility_Sprint_Active(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    ActivationPolicy = ELyraAbilityActivationPolicy::Manual;
	bServerRespectsRemoteAbilityCancellation = false;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;

	AbilityTags.AddTag(D1GameplayTags::Ability_Sprint_Active);
	ActivationOwnedTags.AddTag(D1GameplayTags::Status_Sprint);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = D1GameplayTags::Ability_Sprint_Active;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UD1GameplayAbility_Sprint_Active::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}
	
	if (K2_CheckAbilityCost() == false || K2_CheckAbilityCooldown() == false)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	if (UD1AbilityTask_WaitForTick* TickTask = UD1AbilityTask_WaitForTick::WaitForTick(this))
	{
		TickTask->OnTick.AddDynamic(this, &ThisClass::OnTick);
		TickTask->ReadyForActivation();
	}
	
	if (UAbilityTask_WaitInputRelease* InputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true))
	{
		InputReleaseTask->OnRelease.AddDynamic(this, &ThisClass::OnInputReleased);
		InputReleaseTask->ReadyForActivation();
	}

	GetWorld()->GetTimerManager().SetTimer(SprintTimerHandle, this, &ThisClass::OnSprintCommitTick, CommitInterval, true);
}

void UD1GameplayAbility_Sprint_Active::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (RecoveryBlockEffectClass)
	{
		const UGameplayEffect* GameplayEffectCDO = RecoveryBlockEffectClass->GetDefaultObject<UGameplayEffect>();
		ApplyGameplayEffectToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, GameplayEffectCDO, 1);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UD1GameplayAbility_Sprint_Active::OnTick(float DeltaTime)
{
	if (HasAuthority(&CurrentActivationInfo))
	{
		ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
		if (LyraCharacter == nullptr)
			return;
		
		UCharacterMovementComponent* CharacterMovement = LyraCharacter->GetCharacterMovement();
		if (CharacterMovement == nullptr)
			return;
			
		FVector Direction = UKismetMathLibrary::Normal(CharacterMovement->Velocity);
		FTransform Transform = LyraCharacter->GetActorTransform();
		FVector LocalDirection = UKismetMathLibrary::InverseTransformDirection(Transform, Direction);

		if (LocalDirection.X <= ForwardThreshold)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}
	}
	else
	{
		ED1Direction Direction;
		FVector MovementVector;
		GetMovementDirection(Direction, MovementVector);

		if (Direction != ED1Direction::Forward)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}
	}
}

void UD1GameplayAbility_Sprint_Active::OnInputReleased(float TimeHeld)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UD1GameplayAbility_Sprint_Active::OnSprintCommitTick()
{
	if (K2_CommitAbilityCost() == false)
	{
		GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);
		K2_CommitAbilityCooldown(false, true);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
