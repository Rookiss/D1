// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraCharacterMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "D1GameplayTags.h"
#include "AbilitySystem/Attributes/LyraCombatSet.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraCharacterMovementComponent)

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_MovementStopped, "Gameplay.MovementStopped");

namespace LyraCharacter
{
	static float GroundTraceDistance = 100000.0f;
	FAutoConsoleVariableRef CVar_GroundTraceDistance(TEXT("LyraCharacter.GroundTraceDistance"), GroundTraceDistance, TEXT("Distance to trace down when generating ground information."), ECVF_Cheat);
};


ULyraCharacterMovementComponent::ULyraCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ULyraCharacterMovementComponent::SimulateMovement(float DeltaTime)
{
	if (bHasReplicatedAcceleration)
	{
		// Preserve our replicated acceleration
		const FVector OriginalAcceleration = Acceleration;
		Super::SimulateMovement(DeltaTime);
		Acceleration = OriginalAcceleration;
	}
	else
	{
		Super::SimulateMovement(DeltaTime);
	}
}

bool ULyraCharacterMovementComponent::CanAttemptJump() const
{
	// Same as UCharacterMovementComponent's implementation but without the crouch check
	return IsJumpAllowed() &&
		(IsMovingOnGround() || IsFalling()); // Falling included for double-jump and non-zero jump hold time, but validated by character.
}

void ULyraCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

const FLyraCharacterGroundInfo& ULyraCharacterMovementComponent::GetGroundInfo()
{
	if (!CharacterOwner || (GFrameCounter == CachedGroundInfo.LastUpdateFrame))
	{
		return CachedGroundInfo;
	}

	if (MovementMode == MOVE_Walking)
	{
		CachedGroundInfo.GroundHitResult = CurrentFloor.HitResult;
		CachedGroundInfo.GroundDistance = 0.0f;
	}
	else
	{
		const UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
		check(CapsuleComp);

		const float CapsuleHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
		const ECollisionChannel CollisionChannel = (UpdatedComponent ? UpdatedComponent->GetCollisionObjectType() : ECC_Pawn);
		const FVector TraceStart(GetActorLocation());
		const FVector TraceEnd(TraceStart.X, TraceStart.Y, (TraceStart.Z - LyraCharacter::GroundTraceDistance - CapsuleHalfHeight));

		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(LyraCharacterMovementComponent_GetGroundInfo), false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(QueryParams, ResponseParam);

		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel, QueryParams, ResponseParam);

		CachedGroundInfo.GroundHitResult = HitResult;
		CachedGroundInfo.GroundDistance = LyraCharacter::GroundTraceDistance;

		if (MovementMode == MOVE_NavWalking)
		{
			CachedGroundInfo.GroundDistance = 0.0f;
		}
		else if (HitResult.bBlockingHit)
		{
			CachedGroundInfo.GroundDistance = FMath::Max((HitResult.Distance - CapsuleHalfHeight), 0.0f);
		}
	}

	CachedGroundInfo.LastUpdateFrame = GFrameCounter;

	return CachedGroundInfo;
}

void ULyraCharacterMovementComponent::SetReplicatedAcceleration(const FVector& InAcceleration)
{
	bHasReplicatedAcceleration = true;
	Acceleration = InAcceleration;
}

FRotator ULyraCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		if (ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped))
		{
			return FRotator(0,0,0);
		}
	}

	return Super::GetDeltaRotation(DeltaTime);
}

float ULyraCharacterMovementComponent::GetMaxSpeed() const
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		if (ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped))
			return 0;
		
		const UAttributeSet* AttributeSet = ASC->GetAttributeSet(ULyraCombatSet::StaticClass());
		if (const ULyraCombatSet* CombatSet = Cast<ULyraCombatSet>(AttributeSet))
		{
			float MoveSpeed = CombatSet->GetMoveSpeed();
			float Agility = CombatSet->GetAgility();
			float MoveSpeedPercent = CombatSet->GetMoveSpeedPercent();
			
			float MaxMoveSpeed = FMath::Max(0.f, MoveSpeed + Agility);
			MaxMoveSpeed = FMath::Max(0.f, MaxMoveSpeed + MaxMoveSpeed * (MoveSpeedPercent / 100.f));
			
			switch(MovementMode)
			{
			case MOVE_Walking:
			case MOVE_NavWalking:
			{
				float DirectionCheck;
				float DirectionDot = GetOwner()->GetActorForwardVector().Dot(Velocity.GetSafeNormal());
				if (DirectionDot < 0.25f)
				{
					if (DirectionDot > -0.25f)
					{
						DirectionCheck = MaxMoveSpeed * LeftRightMovePercent;
					}
					else
					{
						DirectionCheck = MaxMoveSpeed * BackwardMovePercent;
					}
				}
				else
				{
					DirectionCheck = MaxMoveSpeed;
				}

				bool IsADS = ASC->HasMatchingGameplayTag(D1GameplayTags::Status_ADS);
				bool IsBlock = ASC->HasMatchingGameplayTag(D1GameplayTags::Status_Block);
				bool IsSprint = ASC->HasMatchingGameplayTag(D1GameplayTags::Status_Sprint);
					
				float CrouchCheck = IsCrouching() ? DirectionCheck * CrouchMovePercent : DirectionCheck;
				float ADSCheck = (IsADS || IsBlock) ? CrouchCheck * ADSMovePercent : CrouchCheck;
				float SprintCheck = IsSprint ? ADSCheck * 1.5f : ADSCheck;

				GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Red, FString::Printf(TEXT("MaxSpeed: %f"), SprintCheck));
				return SprintCheck;
			}
			case MOVE_Falling:
				return MaxMoveSpeed;
			case MOVE_Swimming:
				return MaxSwimSpeed;
			case MOVE_Flying:
				return MaxFlySpeed;
			case MOVE_Custom:
				return MaxCustomMovementSpeed;
			case MOVE_None:
			default:
				return 0.f;
			}
		}
	}
	return Super::GetMaxSpeed();
}
