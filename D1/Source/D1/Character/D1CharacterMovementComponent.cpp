#include "D1CharacterMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "D1Character.h"
#include "AbilitySystem/Attributes/D1AttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1CharacterMovementComponent)

UD1CharacterMovementComponent::UD1CharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

float UD1CharacterMovementComponent::GetMaxSpeed() const
{
	AD1Character* Character = Cast<AD1Character>(GetOwner());
	if (Character == nullptr)
		return 0.f;
	
	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	if (ASC == nullptr)
		return 0.f;

	float MoveSpeed = ASC->GetNumericAttribute(UD1AttributeSet::GetMoveSpeedAttribute());
	float MoveSpeedPercent = ASC->GetNumericAttribute(UD1AttributeSet::GetMoveSpeedPercentAttribute());
	float FinalMoveSpeed = FMath::Max(0.f, MoveSpeed + (MoveSpeed * MoveSpeedPercent / 100.f));
	
	switch(MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
		return IsCrouching() ? FinalMoveSpeed / 2.f : FinalMoveSpeed;
	default:
		return 0.f;
	}
}
