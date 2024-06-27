#include "D1GameplayAbility_Dash.h"

#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Dash)

UD1GameplayAbility_Dash::UD1GameplayAbility_Dash(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

UAnimMontage* UD1GameplayAbility_Dash::SelectDirectionalMontage(ED1Direction MovementDirection) const
{
	UAnimMontage* AnimMontage = nullptr;
	
	switch (MovementDirection)
	{
	case ED1Direction::None:		AnimMontage = nullptr;			break;
	case ED1Direction::Forward:		AnimMontage = nullptr;			break;
	case ED1Direction::Right:		AnimMontage = RightMontage;		break;
	case ED1Direction::Backward:	AnimMontage = BackwardMontage;	break;
	case ED1Direction::Left:		AnimMontage = LeftMontage;		break;
	}

	return AnimMontage;
}
