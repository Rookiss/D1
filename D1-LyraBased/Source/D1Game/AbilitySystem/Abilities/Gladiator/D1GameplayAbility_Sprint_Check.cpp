#include "D1GameplayAbility_Sprint_Check.h"

#include "D1GameplayTags.h"
#include "Character/LyraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Sprint_Check)

UD1GameplayAbility_Sprint_Check::UD1GameplayAbility_Sprint_Check(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	AbilityTags.AddTag(D1GameplayTags::Ability_Sprint_Check);
}

void UD1GameplayAbility_Sprint_Check::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (IsLocallyControlled() == false)
		return;

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
	
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	if (AbilitySystem == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	ED1Direction Direction;
	FVector MovementVector;
	GetMovementDirection(Direction, MovementVector);
	
	bool bSuccessfullyFoundAttribute = false;
	float RemainValue = UAbilitySystemBlueprintLibrary::GetFloatAttributeFromAbilitySystemComponent(AbilitySystem, SprintCostAttribute, bSuccessfullyFoundAttribute);
	
	if (CharacterMovement->IsFalling() || Direction != ED1Direction::Forward || bSuccessfullyFoundAttribute == false || RemainValue <= 10.f)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		Server_RequestSprint(Direction, MovementVector);
		SendSprintActiveEvent(Direction, MovementVector);
	}
}

void UD1GameplayAbility_Sprint_Check::Server_RequestSprint_Implementation(ED1Direction Direction, FVector MovementVector)
{
	SendSprintActiveEvent(Direction, MovementVector);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UD1GameplayAbility_Sprint_Check::SendSprintActiveEvent(ED1Direction Direction, FVector MovementVector)
{
	FHitResult HitResult;
	HitResult.Item = (int32)Direction;
	HitResult.Normal = MovementVector;

	FGameplayEventData Payload;
	Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(HitResult);

	SendGameplayEvent(D1GameplayTags::Ability_Sprint_Active, Payload);
}
