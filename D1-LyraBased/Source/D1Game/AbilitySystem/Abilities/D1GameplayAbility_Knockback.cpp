#include "D1GameplayAbility_Knockback.h"

#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Knockback)

UD1GameplayAbility_Knockback::UD1GameplayAbility_Knockback(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

UAnimMontage* UD1GameplayAbility_Knockback::SelectDirectionalMontage(AActor* Source, AActor* Target) const
{
	if (Source == nullptr || Target == nullptr)
		return nullptr;

	const FRotator& SourceRotator = UKismetMathLibrary::Conv_VectorToRotator(Source->GetActorForwardVector());
	const FRotator& TargetRotator = UKismetMathLibrary::Conv_VectorToRotator(Target->GetActorForwardVector());

	const FRotator& DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(SourceRotator, TargetRotator);
	float YawAbs = FMath::Abs(DeltaRotator.Yaw);

	UAnimMontage* SelectedMontage;
	
	if (YawAbs < 50.f)
	{
		SelectedMontage = BackwardMontage;
	}
	else if (YawAbs > 130.f)
	{
		SelectedMontage = ForwardMontage;
	}
	else if (DeltaRotator.Yaw < 0.f)
	{
		SelectedMontage = LeftMontage;
	}
	else
	{
		SelectedMontage = RightMontage;
	}

	return SelectedMontage;
}
