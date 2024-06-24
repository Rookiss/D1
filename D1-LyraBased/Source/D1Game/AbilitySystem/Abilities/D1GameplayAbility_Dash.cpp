#include "D1GameplayAbility_Dash.h"

#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Camera/LyraCameraComponent.h"
#include "Character/LyraCharacter.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(D1GameplayAbility_Dash)

UD1GameplayAbility_Dash::UD1GameplayAbility_Dash(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UD1GameplayAbility_Dash::GetDirection(FVector& OutFacing, FVector& OutLastMovementInput, FVector& OutMovement) const
{
	if (ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo())
	{
		OutFacing = LyraCharacter->GetActorForwardVector();
		
		if (UAIBlueprintHelperLibrary::GetAIController(LyraCharacter))
		{
			if (UNavigationSystemV1* NavigationSystemV1 = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
			{
				FNavLocation OutProjectedLocation;
				const FVector CharacterLocation = LyraCharacter->GetActorLocation();
				const FVector QueryExtent = FVector(500.f, 500.f, 500.f);
				NavigationSystemV1->ProjectPointToNavigation(CharacterLocation, OutProjectedLocation, QueryExtent);

				FVector CharacterToProjectedXY = (OutProjectedLocation.Location - CharacterLocation) * FVector(1.f, 1.f, 0.f);
				OutMovement = OutLastMovementInput = CharacterToProjectedXY.GetSafeNormal();
			}
		}
		else
		{
			OutLastMovementInput = LyraCharacter->GetLastMovementInputVector();

			if (ULyraCameraComponent* CameraComponent = LyraCharacter->GetCameraComponent())
			{
				OutMovement = CameraComponent->GetForwardVector();
			}
		}
	}
}

UAnimMontage* UD1GameplayAbility_Dash::SelectDirectionalMontage(const FVector& FacingDirection, const FVector& MovementDirection, bool& bOutForwardMovement, bool& bOutLeftMovement) const
{
	const FRotator& FacingRotator = UKismetMathLibrary::Conv_VectorToRotator(FacingDirection);
	const FRotator& MovementRotator = UKismetMathLibrary::Conv_VectorToRotator(MovementDirection);

	const FRotator& DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(FacingRotator, MovementRotator);
	float YawAbs = FMath::Abs(DeltaRotator.Yaw);

	UAnimMontage* SelectedMontage;
	
	if (YawAbs < 89.f)
	{
		SelectedMontage = nullptr;
		bOutForwardMovement = true;
		bOutLeftMovement = false;
	}
	else if (YawAbs > 135.f)
	{
		SelectedMontage = BackwardMontage;
		bOutForwardMovement = false;
		bOutLeftMovement = false;
	}
	else if (DeltaRotator.Yaw < 0.f)
	{
		SelectedMontage = LeftMontage;
		bOutForwardMovement = false;
		bOutLeftMovement = true;
	}
	else
	{
		SelectedMontage = RightMontage;
		bOutForwardMovement = false;
		bOutLeftMovement = false;
	}

	return SelectedMontage;
}
