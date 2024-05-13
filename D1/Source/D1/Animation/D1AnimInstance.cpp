#include "D1AnimInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "KismetAnimationLibrary.h"
#include "Character/D1Player.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/D1PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1AnimInstance)

UD1AnimInstance::UD1AnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UD1AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializedWithAbilitySystem(ASC);
		}
	}
}

void UD1AnimInstance::InitializedWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	GameplayTagPropertyMap.Initialize(this, ASC);
}

void UD1AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (AD1Player* Character = Cast<AD1Player>(TryGetPawnOwner()))
	{
		// AimPitch = Character->CalculateAimPitch();
			
		if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
		{
			Velocity = MovementComponent->Velocity;
			GroundSpeed = Velocity.Size2D();
			Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Character->GetActorRotation());
				
			bShouldMove = (GroundSpeed > 3.f && MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector);
			bIsCrouching = MovementComponent->IsCrouching();
			bIsFalling = MovementComponent->IsFalling();
		}
	}
}
