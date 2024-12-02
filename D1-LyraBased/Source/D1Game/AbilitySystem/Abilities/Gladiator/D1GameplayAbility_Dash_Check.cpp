#include "D1GameplayAbility_Dash_Check.h"

#include "D1GameplayTags.h"
#include "Character/LyraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Dash_Check)

UD1GameplayAbility_Dash_Check::UD1GameplayAbility_Dash_Check(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	AbilityTags.AddTag(D1GameplayTags::Ability_Dash_Check);
}

void UD1GameplayAbility_Dash_Check::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (IsLocallyControlled() == false)
		return;

	ED1Direction Direction;
	FVector MovementVector;
	GetMovementDirection(Direction, MovementVector);

	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	if (LyraCharacter == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	UCharacterMovementComponent* CharacterMovement = LyraCharacter->GetCharacterMovement();
	if (CharacterMovement == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	if (CharacterMovement->IsFalling() || Direction == ED1Direction::None || Direction == ED1Direction::Forward)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		Server_RequestDash(Direction, MovementVector);
		SendDashActiveEvent(Direction, MovementVector);
	}
}

void UD1GameplayAbility_Dash_Check::Server_RequestDash_Implementation(ED1Direction Direction, FVector MovementVector)
{
	SendDashActiveEvent(Direction, MovementVector);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UD1GameplayAbility_Dash_Check::SendDashActiveEvent(ED1Direction Direction, FVector MovementVector)
{
	FHitResult HitResult;
	HitResult.Item = (int32)Direction;
	HitResult.Normal = MovementVector;
			
	FGameplayEventData Payload;
	Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(HitResult);
			
	SendGameplayEvent(D1GameplayTags::Ability_Dash_Active, Payload);
}
